#include "motion_control_new.h"

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
    .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C4,
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
    .type = PID_INC, .kp = 10.0, .ki = 0.8, .kd = 0.5,
},
motor_right_pid = {
    .type = PID_INC, .kp = 10.0, .ki = 0.8, .kd = 0.5,
},
motor_rear_pid = {
    .type = PID_INC, .kp = -10.0, .ki = -1.0, .kd = -0.3,
};

// 中间环 角速度，平移速度  
PID
angular_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.01, .kd = 0.1, .error_limit = 100,
},
translation_forward_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.02, .kd = 0.0, .error_limit = 200,
},
translation_left_v_pid = {
    .type = PID_POI, .kp = 1.0, .ki = 0.02, .kd = 0.0, .error_limit = 200,
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
    .type = PID_POI, .kp = 1.0, .ki = 0.00, .kd = 0.0, .error_limit = 100,
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
        .index = RIGHT,             .pwm_channel_pin = PWM2_MODULE3_CHB_D3,   .gpio_dir_pin = D2,
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

// 启动所有电机PWM通道输出，占空比为0
void motor_all_init(void){
    for(int i = 0; i < MOTOR_INDEX_MAX_PLUS_ONE; i++){
        PID_clean(  motors[i].pid_controller);
        pwm_init(   motors[i].pwm_channel_pin,     MOTOR_PWM_FREQUENCY,    0);
        gpio_init(  motors[i].gpio_dir_pin,       GPO,                    1,     GPO_PUSH_PULL);
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
void motor_unpower(MotorIndex index){
    motor_set_duty(index,0);
}
void motor_all_stop(void){
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
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
    // 获取两个正交编码器的速度
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
    float translation_forward_target_x = 0.0f, translation_left_target_x = 0.0f;
    float target_angular = 0.0f;
    // 分情况获取target
    switch(motion_control.motion_mode){
        case LINE_FOLLOW:
            translation_forward_target_x = translation_forward_pid.output;  // 目标平移距离
            translation_left_target_x    = translation_left_pid.output;     // 目标平移距离
            target_angular               = rotation_pid.output;             // 目标旋转角度
            break;
        case CUBE_DISTANCE_POSITION:
            translation_forward_target_x = 0.0f;
            translation_left_target_x = 0.0f;
            target_angular = 0; // TODO
            break;
        case CUBE_ANGLE_POSITION_LEFT:
        case CUBE_ANGLE_POSITION_RIGHT:
            translation_forward_target_x = 0.0f; 
            translation_left_target_x = 0.0f;
            target_angular = 0; // TODO
            break;
        case CUBE_PUSH:
        case LINE_BACK:
            translation_forward_target_x = 0.0f; 
            translation_left_target_x = 0.0f;
            target_angular = 0; // TODO
            break;
        default:
            translation_forward_target_x = 0.0f; 
            translation_left_target_x = 0.0f;
            target_angular = 0; // TODO
    }
    
    // 计算平移速度
    PID_calculate(&translation_forward_v_pid, translation_forward_target_x, 0.0f); // 目标平移速度
    PID_calculate(&translation_left_v_pid,    translation_left_target_x,    0.0f); // 目标平移速度
    // 计算角速度
    PID_calculate(&angular_v_pid, target_angular, 0.0f); // 目标角速度

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
                    translation_forward_pid.output = 70;
                    translation_left_pid.output = 0;
                    break;
                case Zebra:
                    // translation_forward_pid.output = 0;
                    // translation_left_pid.output = 0;
                    break;
                default:
                    translation_forward_pid.output = 60;
                    translation_left_pid.output = 0;
                    break;
            }
            // translation_forward_pid.output = 60;
            // translation_left_pid.output = 0;
            PID_calculate(&rotation_pid, image_result.offset, 0);
            return;
        case CUBE_DISTANCE_POSITION:
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output  = 0.0f;
            break;
        case CUBE_ANGLE_POSITION_LEFT:
        case CUBE_ANGLE_POSITION_RIGHT:
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output  = 0.0f;
            return;
        case CUBE_PUSH:
        case LINE_BACK:
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output  = 0.0f;
            return;
        default:
            translation_forward_pid.output = 0.0f;
            translation_left_pid.output  = 0.0f;
    }
}

uint64 pit_count = 0;

void motion_control_pid_callback(){
    pit_count++;
    if(pit_count % 4 == 0){
        // 最外环  offset -> x
        motor_rotation_translation_pid_calc_apply();
    }
    if(pit_count % 2 == 0){
        // 中间环控制 x -> v
        motor_translation_angular_v_pid_calc_apply();
    }
    // 最内环控制 电机速度稳定控制 target_speed -> pwm_duty_plus
    motor_speed_pid_calc_apply();
}

void motion_control_pid_pit_init(){
    pit_ms_init(PIT_CH1, 2); // 10ms周期
}
