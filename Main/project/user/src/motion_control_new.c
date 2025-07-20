#include "motion_control_new.h"

#define CUBE_RIGHT_CENTER_X  160
#define CUBE_RIGHT_CENTER_Y  170

Encoder encoder_left = {
    .encoder_index      = QTIMER1_ENCODER1,
    .encoder_channel_1  = QTIMER1_ENCODER1_CH1_C0,
    .encoder_channel_2  = QTIMER1_ENCODER1_CH2_C1,
};
Encoder encoder_right = {
    .encoder_index      = QTIMER1_ENCODER2, 
    .encoder_channel_1  = QTIMER1_ENCODER2_CH1_C2,
    .encoder_channel_2  = QTIMER1_ENCODER2_CH2_C24,
};
Encoder encoder_rear = {
    .encoder_index      = QTIMER2_ENCODER1,
    .encoder_channel_1  = QTIMER2_ENCODER1_CH1_C3,
    .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C25,
};

MotionControl motion_mode ={
    .motion_mode        = LINE_FOLLOW,
    .last_motion_mode   = LINE_FOLLOW,
    .line_gyro_angle_z  = 0.0f,
};

// 滤波器
MovingAverageFilter encoder_filter[MOTOR_INDEX_MAX_PLUS_ONE];

// 内环 电机速度PID
PID
motor_left_pid = {
    .type = PID_INC, .kp = 12.0, .ki = 4.0, .kd = 0.5,
},
motor_right_pid = {
    .type = PID_INC, .kp = 12.0, .ki = 4.0, .kd = 0.5,
},
motor_rear_pid = {
    .type = PID_INC, .kp = -12.0, .ki = -4.0, .kd = -0.3,
};

// 中间环 角速度，平移速度  
PID
angular_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.02, .kd = 0.1, .error_limit = 200,
},
translation_forward_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.00, .kd = 0.0, .error_limit = 200,
},
translation_left_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.00, .kd = 0.0, .error_limit = 200,
};

// 外环 平移距离，旋转角度
PID
rotation_pid = {
    .type = PID_POI, .kp = 0.5, .ki = 0.01, .kd = 0.08, .error_limit = 100,
},
translation_forward_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.00, .kd = 0.0, .error_limit = 100,
},
translation_left_pid = {
    .type = PID_POI, .kp = 0.7, .ki = 0.00, .kd = 0.0, .error_limit = 100,
};


Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
    [LEFT] = 
    {
        .index = LEFT,              .pwm_channel_pin = PWM2_MODULE0_CHB_C7,   .gpio_dir_pin = C6,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_left,   .pid_controller = &motor_left_pid,
    },
    [RIGHT] =
    {
        .index = RIGHT,             .pwm_channel_pin = PWM2_MODULE1_CHA_C8,   .gpio_dir_pin = C9,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_right,  .pid_controller = &motor_right_pid,
    },
    [REAR] =
    {
        .index = REAR,              .pwm_channel_pin = PWM2_MODULE2_CHB_C11,  .gpio_dir_pin = C10,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_rear,   .pid_controller = &motor_rear_pid,
    },
};

// TARGET MOTION
MotionControl motion_control = {
    .motion_mode        = LINE_FOLLOW,  .last_motion_mode = LINE_FOLLOW,        .line_gyro_angle_z  = 0.0f,
};

uint8 motor_rotation_translation_pid_calc_flag = 0;
uint8 motor_translation_angular_v_pid_calc_flag = 0;
uint8 motor_speed_pid_calc_flag = 0;

uint32 image_identify_wait_start_time; // 立方体识别等待开始时间
uint8  image_identify_wait_flag = 0; // 立方体识别等待标志位

CubePushDir cube_push_dir = CUBE_PUSH_DIR_LEFT; // 立方体推方向

uint8  motion_control_target_angle_z_setted_flag = 0; // 立方体角度定位目标角度设置标志位

// 启动所有电机PWM通道输出，占空比为0
void motor_all_init(void){
    for(int i = 0; i < MOTOR_INDEX_MAX_PLUS_ONE; i++){
        PID_clean(  motors[i].pid_controller);
        pwm_init(   motors[i].pwm_channel_pin,     MOTOR_PWM_FREQUENCY,    0);
        gpio_init(  motors[i].gpio_dir_pin,        GPO,                    1,     GPO_PUSH_PULL);
    }
}

// 设置电机PWM通道输出占空比
void motor_set_duty(MotorIndex index, int32 duty){
    // 占空比限幅
    if(abs(duty) > MOTOR_PWM_DUTY_MAX){
        (duty > 0) ? (duty = MOTOR_PWM_DUTY_MAX) : (duty = -MOTOR_PWM_DUTY_MAX);
    }
    if(duty > 0){
        gpio_set_level(motors[index].gpio_dir_pin, 1);
    }else if(duty < 0){
        gpio_set_level(motors[index].gpio_dir_pin, 0);
    }
    pwm_set_duty(motors[index].pwm_channel_pin,    abs(duty));
    // 记录占空比
    motors[index].pwm_duty = duty;
}

void motor_plus_duty(MotorIndex index, int32 delta_duty){
    if(motors[index].pid_controller->error_zero_count >= 300){
        motor_set_duty(index, 0);
    }
    motor_set_duty(index, motors[index].pwm_duty + delta_duty);
}

void motor_run_with_speed(MotorIndex index, int32 speed){
    motors[index].set_speed = speed;
}
void motor_all_unpower(){
    // 停止所有PID运算
    motor_rotation_translation_pid_calc_flag = 0; // 外环PID
    motor_translation_angular_v_pid_calc_flag = 0; // 中环PID
    motor_speed_pid_calc_flag = 0; // 内环PID
    // 停止所有电机动力
    for(MotorIndex index = 0; index < MOTOR_INDEX_MAX_PLUS_ONE; index ++){
        motor_set_duty(index,0);
    }
}
void motor_all_stop(void){
    // return ;
    // 停止外环和中环PID运算
    motor_rotation_translation_pid_calc_flag = 0; // 外环PID
    motor_translation_angular_v_pid_calc_flag = 0; // 中环PID
    // system_delay_ms(10); // 等待一段时间，确保电机停止
    // 设置速度为0
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE; index ++){
        motors[index].set_speed = 0;
    }
}

// 滑动窗口移动平均滤波器
void MAF_Init(MovingAverageFilter *filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->index = 0;
    filter->sum = 0;
}

int16 MAF_Update(MovingAverageFilter *filter, int16 new_value) {
    // 减去最旧的值
    filter->sum -= filter->buffer[filter->index];
    // 添加新值
    filter->buffer[filter->index] = new_value;
    filter->sum += new_value;
    // 更新索引
    filter->index = (filter->index + 1) % WINDOW_SIZE;
    // 返回平均值
    return filter->sum / WINDOW_SIZE;
}

// Motor encoder
void encoder_all_init(){
    encoder_quad_init(encoder_left.encoder_index,   encoder_left.encoder_channel_1,     encoder_left.encoder_channel_2);
    encoder_quad_init(encoder_right.encoder_index,  encoder_right.encoder_channel_1,    encoder_right.encoder_channel_2);
    encoder_quad_init(encoder_rear.encoder_index,   encoder_rear.encoder_channel_1,     encoder_rear.encoder_channel_2);
    MAF_Init(&encoder_filter[LEFT]);
    MAF_Init(&encoder_filter[RIGHT]);
    MAF_Init(&encoder_filter[REAR]);
}
void motor_encoder_pit_init(void){
    pit_ms_init(MOTOR_ENCODER_PIT,MOTOR_ENCODER_PIT_TIME);
}
void motor_encoder_pit_callback(void){
    // 获取正交编码器的速度
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
        // 获取速度
        motors[index].current_speed = MAF_Update( &encoder_filter[index], encoder_get_speed(motors[index].encoder->encoder_index) );
        // 清除计数
        encoder_clear_count(motors[index].encoder->encoder_index);
    }
}

// 最内环控制 电机速度稳定控制 target_speed -> pwm_duty_plus
void motor_speed_pid_calc_apply(){
    for(MotorIndex index = 0; index < MOTOR_INDEX_MAX_PLUS_ONE; index++){
        // PID计算
        PID_calculate(motors[index].pid_controller,
                    (float)motors[index].set_speed, 
                    (float)motors[index].current_speed);
        // 输出到电机
        motor_plus_duty(index, (int32)(motors[index].pid_controller->output));
    }
}

// 中间环控制 x -> v
void motor_translation_angular_v_pid_calc_apply(){
    // translation
    // float translation_forward_target_x = 0.0f, translation_left_target_x = 0.0f;
    // float target_angular = 0.0f;
    // // 分情况获取target
    // switch(motion_control.motion_mode){
    //     case LINE_FOLLOW:
    //     case CUBE_DISTANCE_POSITION:
    //     case CUBE_ANGLE_POSITION:
    //         translation_forward_target_x = translation_forward_pid.output;  // 目标平移距离
    //         translation_left_target_x    = translation_left_pid.output;     // 目标平移距离
    //         target_angular               = rotation_pid.output;             // 目标旋转角度
    //         break;
    //     case CUBE_IDENTIFY:
    //         translation_forward_v_pid.output = 0.0f; // 立方体识别时不需要前进
    //         translation_left_v_pid.output = 0.0f; // 立方体识别时不需要左移
    //         angular_v_pid.output = 0.0f; // 立方体识别时不需要旋转
    //         goto V_APPLY; // 直接跳转到V_APPLY
    //         break;
    //     case CUBE_PUSH:
    //         translation_forward_pid.output = 30.0f; // 推箱子时前进30
    //         translation_left_pid.output = 0.0f; // 推箱子时不需要左移
    //         goto V_APPLY; // 直接跳转到V_APPLY
    //         break;
    //     case LINE_POSITION_BACK:
    //         translation_forward_v_pid.output = 0.0f; // 推箱子时不需要前进
    //         translation_left_v_pid.output = 30.0f;
    //         angular_v_pid.output = -10.0f;
    //         goto V_APPLY; // 直接跳转到V_APPLY
    //         break;
    //     default:
    //         translation_forward_target_x = 0.0f; 
    //         translation_left_target_x = 0.0f;
    //         target_angular = 0; // TODO
    // }
    // 计算平移速度
    // PID_calculate(&translation_forward_v_pid, translation_forward_target_x, 0.0f); // 目标平移速度
    // PID_calculate(&translation_left_v_pid,    translation_left_target_x,    0.0f); // 目标平移速度
    // // 计算角速度
    // PID_calculate(&angular_v_pid, target_angular, 0.0f); // 目标角速度
    PID_calculate(&translation_forward_v_pid, translation_forward_pid.output, 0.0f); // 目标平移速度
    PID_calculate(&translation_left_v_pid,    translation_left_pid.output,    0.0f); // 目标平移速度
    // 计算角速度
    PID_calculate(&angular_v_pid, rotation_pid.output, 0.0f); // 目标角速度
V_APPLY:
    // 计算电机速度
    motors[LEFT].set_speed    =   (int32) (-translation_forward_v_pid.output * COS_PI_D_6 + translation_left_v_pid.output * SIN_PI_D_6 - angular_v_pid.output); // 左侧电机速度需要加上角速度
    motors[RIGHT].set_speed   =   (int32) ( translation_forward_v_pid.output * COS_PI_D_6 + translation_left_v_pid.output * SIN_PI_D_6 - angular_v_pid.output); // 右侧电机速度需要加上角速度
    motors[REAR].set_speed    =   (int32) ( translation_left_v_pid.output + angular_v_pid.output); // 后面方向电机速度需要加上角速度
}

// 最外环  offset -> x
void motor_rotation_translation_pid_calc_apply(){
    switch (motion_control.motion_mode)
    {
        case LINE_FOLLOW:
            // 巡线需要根据赛道元素决定速度
            switch (image_result.element_type)
            {
                case Normal:
                case Cross:
                    translation_forward_pid.output = 80;
                    translation_left_pid.output = 0;
                    break;
                case Zebra:
                    translation_forward_pid.output = 0;
                    translation_left_pid.output = 0;
                    break;
                default:
                    translation_forward_pid.output = 70;
                    translation_left_pid.output = 0;
                    break;
            }
            // translation_forward_pid.output = 60;
            // translation_left_pid.output = 0;
            if(image_result.offset > 70){
                image_result.offset = 70; // 限制偏移量
            }else if (image_result.offset < -70){
                image_result.offset = -70; // 限制偏移量
            }
            PID_calculate(&rotation_pid, image_result.offset, 0);
            return;
            break;
        case CUBE_IDENTIFY:
            // 立方体识别模式下，静止
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output = 0.0f;
            rotation_pid.output = 0.0f;
            break;
        case CUBE_DISTANCE_POSITION:
            translation_forward_pid.output  = 1.0 * (20000.0 - cube_info.p_count) / 1000.0;
            translation_left_pid.output     = (cube_info.x_center - 160) / 4.0;
            // if(image_result.offset > 70){
            //     image_result.offset = 70; // 限制偏移量
            // }else if (image_result.offset < -70){
            //     image_result.offset = -70; // 限制偏移量
            // }
            // PID_calculate(&rotation_pid, image_result.offset, 0);
            rotation_pid.output = 0.0f; // 立方体距离定位模式下不需要旋转
            return;
            break;
        case CUBE_ANGLE_POSITION:
            if (motion_control.target_gyro_angle_z - gyroscope_result.angle_z > 0) {
                // 顺时针旋转
                translation_left_pid.output = 15.0f; // 左侧移动
            } else {
                translation_left_pid.output = -15.0f; // 右侧移动
            }
            translation_forward_pid.output = 0.0f; // 目标前进速度为0
            rotation_pid.output = motion_control.target_gyro_angle_z - gyroscope_result.angle_z; // 自转
            break;
        case CUBE_PUSH:
            translation_forward_pid.output = 30.0f; // 推箱子时前进30
            translation_left_pid.output = 0.0f; // 推箱子时不需要左移
            if (cube_info.state == CUBE_INSIDE_VIEW) {
                rotation_pid.output = cube_info.x_center - CUBE_RIGHT_CENTER_X; // 推箱子时自转角度为箱子中心到右侧中心的偏移量
            }
            break;
        case LINE_POSITION_BACK:
            translation_forward_pid.output = -30.0f; // 后退30
            break;
        case LINE_ANGLE_BACK:
            rotation_pid.output = motion_control.line_gyro_angle_z - gyroscope_result.angle_z; // 自转
            break;
        default:
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output  = 0.0f;
            rotation_pid.output = 0.0f; // 默认不动
    }
    // rotation_pid.output = 20.0f; // 自转 顺时针旋转为正
    // translation_forward_pid.output = 0.0f;
    // translation_left_pid.output  = 0.0f;
}

uint64 pit_count = 0;

void motion_control_pid_callback(){
    pit_count++;
    if(pit_count % 4 == 0 && motor_rotation_translation_pid_calc_flag){
        // 最外环  offset -> x
        motor_rotation_translation_pid_calc_apply();
    }
    if(pit_count % 2 == 0 && motor_translation_angular_v_pid_calc_flag){
        // 中间环控制 x -> v
        motor_translation_angular_v_pid_calc_apply();
    }
    // 最内环控制 电机速度稳定控制 target_speed -> pwm_duty_plus
    if(motor_speed_pid_calc_flag){
        motor_speed_pid_calc_apply();
    }
}

void motion_control_pid_pit_init(){
    // 打开PID运算flag
    motor_rotation_translation_pid_calc_flag = 1; // 外环PID
    motor_translation_angular_v_pid_calc_flag = 1; // 中环PID
    motor_speed_pid_calc_flag = 1; // 内环PID
    // 启动PID定时器
    pit_ms_init(MOTOR_PID_PIT, MOTOR_PID_PIT_TIME);
}

// 该函数为阻塞函数，不宜放在PIT中
void motion_mode_calc(){
    switch(motion_control.motion_mode){
        case LINE_FOLLOW:
            // 巡线模式下，判断是否看到了箱子
            if (cube_info.state == CUBE_INSIDE_VIEW) {
                // 先停车
                // motor_all_stop();
                motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                motion_control.motion_mode = CUBE_DISTANCE_POSITION; // 进入立方体距离定位模式
                // while(1);
            }
            break;
        case CUBE_DISTANCE_POSITION:
            // 立方体距离定位模式下，判断是否看到了箱子
            if (cube_info.state == CUBE_OUTSIDE_VIEW) {
                // 没有看到箱子，回到巡线模式
                motion_control.motion_mode = LINE_FOLLOW;
                return;
            }
            if (abs(cube_info.x_center - 160) < 20 && abs(20000 - cube_info.p_count) < 1500) {
                // 如果箱子在视野中心附近，进入立方体识别模式
                motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                motion_control.motion_mode = CUBE_IDENTIFY;
            }
            break;
        case CUBE_IDENTIFY:
            // 立方体识别模式下，判断是否看到了箱子
            if (cube_info.state == CUBE_OUTSIDE_VIEW) {
                // 没有看到箱子，回到巡线模式
                image_identify_wait_flag = 0; // 重置等待标志
                motion_control.motion_mode = LINE_FOLLOW;
                return;
            }
            // 识别等待
            if (!image_identify_wait_flag){
                image_identify_wait_start_time = timer_get(GPT_TIM_1); // 获取等待开始时间
                image_identify_wait_flag = 1; // 设置等待标志
            } else {
                if (timer_get(GPT_TIM_1) - image_identify_wait_start_time >= IMAGE_IDENTIFY_WAIT_TIME) {
                    // 等待超过设定时间，首先添加识别结果
                    cube_info_add(current_cube_face_info.class, current_cube_face_info.number);
                    // 然后根据识别结果决定推离方向
                    cube_push_dir = get_cube_push_dir(current_cube_face_info.class, current_cube_face_info.number);
                    motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                    image_identify_wait_flag = 0; // 重置等待标志
                    motion_control.motion_mode = CUBE_ANGLE_POSITION; // 进入立方体角度定位模式
                }
            }
            break;
        case CUBE_ANGLE_POSITION:
            // 先记录赛道的角度
            motion_control.line_gyro_angle_z = gyroscope_result.angle_z;
            if (!motion_control_target_angle_z_setted_flag) {
                if (cube_push_dir == CUBE_PUSH_DIR_LEFT) {
                // 如果推离方向是左侧，应该绕到箱子右侧
                    motion_control.target_gyro_angle_z = motion_control.line_gyro_angle_z - 90.0f; // 目标角度为当前角度减90度
                } else if (cube_push_dir == CUBE_PUSH_DIR_RIGHT) {
                    // 如果推离方向是右侧，应该绕到箱子左侧
                    motion_control.target_gyro_angle_z = motion_control.line_gyro_angle_z + 90.0f; // 目标角度为当前角度加90度
                }
                motion_control_target_angle_z_setted_flag = 1; // 设置目标角度标志位
            } else {
                // 如果目标角度已经设置过了，判断角度定位完成标志
                if (fabs(motion_control.target_gyro_angle_z - gyroscope_result.angle_z) < 5.0f) {
                    motion_control_target_angle_z_setted_flag = 0; // 重置标志位
                    // 角度定位完成，进入推箱子模式
                    motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                    motion_control.motion_mode = CUBE_PUSH; // 进入推箱子模式
                }
            }
            break;
        case CUBE_PUSH:
            // 推箱子模式下，判断是否出赛道
            if (grayscale_data == GRAY_OUT_TRACK) {
                motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                motion_control.motion_mode = LINE_POSITION_BACK; // 出赛道，进入返回赛道模式
                return;
            }
            break;
        case LINE_POSITION_BACK:
            // 返回赛道模式下，判断是否回到赛道
            if (grayscale_data == GRAY_IN_TRACK) {
                motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                motion_control.motion_mode = LINE_ANGLE_BACK; // 回到巡线模式
            }
            break;
        case LINE_ANGLE_BACK:
            // 返回赛道模式下，判断是否回到赛道
            if (fabs(gyroscope_result.angle_z - motion_control.line_gyro_angle_z) < 5.0f) {
                // 如果回到赛道，回到巡线模式
                motion_control.last_motion_mode = motion_control.motion_mode; // 记录上一个模式
                motion_control.motion_mode = LINE_FOLLOW; // 回到巡线模式
            }
            break;
        default:
            // 如果进入了未知模式，回到巡线模式
            motion_control.motion_mode = LINE_FOLLOW;
            break;
            
    }
}
