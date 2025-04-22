#include "motion_control.h"

uint8 KI_clear_flag[5] = {0,0,0};

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

// 电机速度PID
MotorPID motor_left_pid = {
    .KP = 2.0,          .KI = 0.05,        .KD = 0.9,
    .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
};
MotorPID motor_right_pid = {
    .KP = 2.0,          .KI = 0.05,        .KD = 0.9,
    .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
};
MotorPID motor_rear_pid = {
    .KP = 2.0,          .KI = 0.10,        .KD = 0.9,
    .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
};

// 角速度PID
RotationPID rotation_pid = {
    .KP = -3.0,          .KI = -0.05,        .KD = -1.5,
    .last_offset = 0,    .sum_offset = 0,    .offset = 0,
};

// 平移PID
TranslationPID   translation_pid = {
    .front_KP = 5.0,        .front_KI = 0.02,       .front_KD = 1.0,
    .front_sum_offset = 0,  .front_offset = 0,      .front_last_offset = 0,       .front_speed_out = 0,

    .left_KP = 3.0,         .left_KI = 0.01,        .left_KD = 0.8,
    .left_sum_offset = 0,   .left_offset = 0,       .left_last_offset = 0,        .left_speed_out = 0,
};

// TODO
Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
    {
        .index = LEFT,              .pwm_channel_pin = PWM2_MODULE0_CHB_C7,   .gpio_dir_pin = C6,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_left,   .PID = &motor_left_pid,
    },
    {
        .index = RIGHT,             .pwm_channel_pin = PWM2_MODULE3_CHB_D3,   .gpio_dir_pin = D2,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_right,  .PID = &motor_right_pid,
    },
    {
        .index = REAR,              .pwm_channel_pin = PWM2_MODULE2_CHB_C11,  .gpio_dir_pin = C10,
        .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
        .encoder = &encoder_rear,   .PID = &motor_rear_pid,
    },
};

// TARGET MOTION
MotionControl motion_control = {
    .motion_mode        = LINE_FOLLOW,  .last_motion_mode = LINE_FOLLOW,
    .line_distance      = 0,            .line_gyro_angle_z  = 0.0f,
};

// 启动所有电机PWM通道输出，占空比为0，方向为逆时针
void motor_all_init(void){
    for(int i = 0; i < MOTOR_INDEX_MAX_PLUS_ONE; i++){
        pwm_init(motors[i].pwm_channel_pin,     MOTOR_PWM_FREQUENCY,    0);
        gpio_init(motors[i].gpio_dir_pin,       GPO,                    1,     GPO_PUSH_PULL);
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

void motor_pid_pit_init(void){
    pit_ms_init(MOTOR_PID_PIT,MOTOR_PID_PIT_TIME);
}
void motor_pid_calc_apply(MotorIndex index){
    
    double out = 0.;
    Motor * motor = &motors[index];
    motor->PID->wrong = motor->set_speed - motor->current_speed;
    // 比例
    out += (double) (motor->PID->KP * motor->PID->wrong);

    if(motor->PID->wrong == 0 && KI_clear_flag[index] < 10){
        KI_clear_flag[index]++;
    }else{
        KI_clear_flag[index] = 0;
    }
    if(KI_clear_flag[index] >= 10){
        motors[index].PID->sum_wrong = 0;
        motor_set_duty(index,0);
        KI_clear_flag[index] = 0;
    }
    // 积分
    motor->PID->sum_wrong += motor->PID->wrong;
    // 积分限幅
    if(abs(motor->PID->sum_wrong) > SUM_WRONG_MAX){
        (motor->PID->sum_wrong > 0) ? (motor->PID->sum_wrong = SUM_WRONG_MAX) : (motor->PID->sum_wrong = -SUM_WRONG_MAX);
    }
    out += (double) (motor->PID->KI * motor->PID->sum_wrong);

    // 微分
    out += (double) (motor->PID->KD * (motor->PID->wrong - motor->PID->last_wrong));

    // 记录wrong
    motor->PID->last_wrong = motor->PID->wrong;

    // 输出
    motor_plus_duty(index, (int32)out);
}


void motor_pid_pit_call(void){
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
        motor_pid_calc_apply(index);
    }
}

// Motor encoder
void encoder_all_init(){
    encoder_quad_init(encoder_left.encoder_index,   encoder_left.encoder_channel_1,     encoder_left.encoder_channel_2);
    encoder_quad_init(encoder_right.encoder_index,  encoder_right.encoder_channel_1,    encoder_right.encoder_channel_2);
    encoder_quad_init(encoder_rear.encoder_index,   encoder_rear.encoder_channel_1,     encoder_rear.encoder_channel_2);
}
void motor_encoder_pit_init(void){
    pit_ms_init(MOTOR_ENCODER_PIT,MOTOR_ENCODER_PIT_TIME);
}
void motor_encoder_pit_call(void){
    // 获取两个正交编码器的速度
    for(MotorIndex index = 0;index < REAR;index ++){
        // 获取速度
        motors[index].current_speed = encoder_get_speed(motors[index].encoder->encoder_index);
        // 清除计数
        encoder_clear_count(motors[index].encoder->encoder_index);
    }
    // 获取后面方向编码器速度
    motors[REAR].current_speed = encoder_get_speed(motors[REAR].encoder->encoder_index) / -4;
    // 清除计数
    encoder_clear_count(motors[REAR].encoder->encoder_index);
}

// ROTATION
// 车体旋转PID计算
void rotation_pid_calc(){
    volatile double out = 0.;
    // 分情况获取offset
    switch (motion_control.motion_mode)
    {
    case LINE_FOLLOW:
        rotation_pid.offset = image_result.offset;
        break;
    case CUBE_DISTANCE_POSITION:
        // 需要测试决定
        rotation_pid.offset = image_result.offset;
        // rotation_pid.offset = cube_info.x_offset;
        // rotation_pid.offset = 0;
        break;
    case CUBE_ANGLE_POSITION_LEFT:
        rotation_pid.offset = (int32)(motion_control.line_gyro_angle_z + 90.0f - gyroscope_result.angle_z); // 需要测试决定
        break;
    case CUBE_ANGLE_POSITION_RIGHT:
        rotation_pid.offset = (int32)(motion_control.line_gyro_angle_z - 90.0f - gyroscope_result.angle_z); // 需要测试决定
        break;
    default:
        rotation_pid.offset = 0;
        break;
    }
    // 比例
    out += (double) (rotation_pid.KP * rotation_pid.offset);
    // 积分
    rotation_pid.sum_offset += rotation_pid.offset;
    // 积分清零
    if(rotation_pid.offset == 0){
        rotation_pid.sum_offset = 0;
    }
    // 积分限幅
    if(abs(rotation_pid.sum_offset) > ROTATION_SUM_OFFSET_MAX){
        (rotation_pid.sum_offset > 0) ? (rotation_pid.sum_offset = ROTATION_SUM_OFFSET_MAX) : (rotation_pid.sum_offset = -ROTATION_SUM_OFFSET_MAX);
    }
    // 积分
    out += (double) (rotation_pid.KI * rotation_pid.sum_offset);
    // 微分
    out += (double) (rotation_pid.KD * (rotation_pid.offset - rotation_pid.last_offset));

    // 记录wrong
    rotation_pid.last_offset = rotation_pid.offset;

    // 输出
    if(motion_control.motion_mode == CUBE_ANGLE_POSITION_LEFT || motion_control.motion_mode == CUBE_ANGLE_POSITION_RIGHT){
        if(abs(out) > 150.0){
            (out > 0) ? (out = 150.0) : (out = -150.0);
        }
    }
    rotation_pid.wl_out = (int32)out;

}

void translation_pid_calc(void){
    // 分情况获取offset，或者直接输出speed_out
    switch (motion_control.motion_mode)
    {
    case LINE_FOLLOW:
        // 巡线需要根据赛道元素决定速度
        switch (image_result.element_type)
        {
        case Normal:
        case Cross:
            translation_pid.front_speed_out = 800;
            break;
        case Zebra:
            translation_pid.front_speed_out = 0;
            break;
        case CurveLeft:
        case CurveRight:
        case LoopLeft:
        case LoopRight:
            translation_pid.front_speed_out = 700;
            break;
        default:
            break;
        }
        translation_pid.left_speed_out = 0;

        // PID积分清零
        translation_pid.front_sum_offset = 0;
        translation_pid.left_sum_offset  = 0;
        return;
        break;
    case CUBE_DISTANCE_POSITION:
    case CUBE_ANGLE_POSITION_LEFT:
    case CUBE_ANGLE_POSITION_RIGHT:
        // translation_pid.front_offset = cube_info.y_offset;
        // translation_pid.left_offset  = cube_info.x_offset;
        translation_pid.front_speed_out = 0;
        translation_pid.left_speed_out  = 0;
        return;
        break;
    case CUBE_PUSH:
        translation_pid.front_offset = (10000 - motion_control.line_distance) / 100;
        translation_pid.left_offset  = 0;
    case LINE_BACK:
        translation_pid.front_offset = -motion_control.line_distance / 100;
        break;
    default:
        translation_pid.left_speed_out  = 0;
        translation_pid.front_speed_out = 0;
        return;
        break;
    }
    // 获取到了offset，进行PID计算
    double left_speed_out = 0. ,front_speed_out = 0.;
    // ===========================比例===========================
    front_speed_out += (double) (translation_pid.front_KP   * translation_pid.front_offset);
    left_speed_out  += (double) (translation_pid.left_KP    * translation_pid.left_offset);
    // ===========================积分===========================
    translation_pid.front_sum_offset += translation_pid.front_offset;
    translation_pid.left_sum_offset  += translation_pid.left_offset;
    // 积分清零
    if(translation_pid.front_offset == 0){
        translation_pid.front_sum_offset = 0;
    }
    if(translation_pid.left_offset == 0){
        translation_pid.left_sum_offset = 0;
    }
    // 积分限幅
    if(abs(translation_pid.front_sum_offset) > TRANSLATION_SUM_OFFSET_MAX){
        (translation_pid.front_sum_offset > 0) ? (translation_pid.front_sum_offset = ROTATION_SUM_OFFSET_MAX) : (translation_pid.front_sum_offset = -ROTATION_SUM_OFFSET_MAX);
    }
    if(abs(translation_pid.left_sum_offset) > TRANSLATION_SUM_OFFSET_MAX){
        (translation_pid.left_sum_offset > 0) ? (translation_pid.left_sum_offset = ROTATION_SUM_OFFSET_MAX) : (translation_pid.left_sum_offset = -ROTATION_SUM_OFFSET_MAX);
    }
    // 积分计算
    front_speed_out += (double) (translation_pid.front_KI * translation_pid.front_sum_offset);
    left_speed_out  += (double) (translation_pid.left_KI  * translation_pid.left_sum_offset);

    // ===========================微分===========================
    front_speed_out += (double) (translation_pid.front_KD * (translation_pid.front_offset - translation_pid.front_last_offset));
    left_speed_out  += (double) (translation_pid.left_KD  * (translation_pid.left_offset - translation_pid.left_last_offset));

    // 记录offset
    translation_pid.front_last_offset = translation_pid.front_offset;
    translation_pid.left_last_offset  = translation_pid.left_offset;

    // 输出
    translation_pid.front_speed_out = (int32)front_speed_out;
    translation_pid.left_speed_out  = (int32)left_speed_out;
}

void motion_pid_pit_init(void){
    pit_ms_init(MOTION_PID_PIT,MOTION_PID_PIT_TIME);
}

void motion_pid_callback(void){
    rotation_pid_calc();
    translation_pid_calc();
}

// 车运动解算函数
void target_motion_calc(void){
    // ====================运动模式与相关参量计算====================
    
    // if(cube_info.state == CUBE_OUTSIDE_VIEW){
    //     motion_control.motion_mode = LINE_FOLLOW;
    // }else{
    //     motion_control.motion_mode = CUBE_DISTANCE_POSITION;
    // }
    if(motion_control.motion_mode == CUBE_ANGLE_POSITION_LEFT && motion_control.last_motion_mode != CUBE_ANGLE_POSITION_LEFT){
        motion_control.line_gyro_angle_z = gyroscope_result.angle_z;
    }else if(motion_control.motion_mode == CUBE_ANGLE_POSITION_RIGHT && motion_control.last_motion_mode != CUBE_ANGLE_POSITION_RIGHT){
        motion_control.line_gyro_angle_z = gyroscope_result.angle_z;
    }
    // ====================电机速度解算====================
    // 初始化三个电机速度缓存
    int32   motor_left_speed    = 0,
            motor_right_speed   = 0,
            motor_rear_speed    = 0;

    motor_left_speed    =   (int32) (-translation_pid.front_speed_out * COS_PI_D_6 + translation_pid.left_speed_out * SIN_PI_D_6);
    motor_right_speed   =   (int32) ( translation_pid.front_speed_out * COS_PI_D_6 - translation_pid.left_speed_out * SIN_PI_D_6);
    motor_rear_speed    =   (int32) ( translation_pid.left_speed_out);

    motor_left_speed    +=  rotation_pid.wl_out;
    motor_right_speed   +=  rotation_pid.wl_out;
    motor_rear_speed    +=  rotation_pid.wl_out;
    
    // 应用速度
    motor_run_with_speed(LEFT,motor_left_speed);
    motor_run_with_speed(RIGHT,motor_right_speed);
    motor_run_with_speed(REAR,motor_rear_speed);

    // 记录运动模式
    motion_control.last_motion_mode = motion_control.motion_mode;
}
