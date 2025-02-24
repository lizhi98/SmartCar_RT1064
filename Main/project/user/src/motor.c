#include "motor.h"

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
Encoder encoder_front = {
    .encoder_index      = QTIMER2_ENCODER1,
    .encoder_channel_1  = QTIMER2_ENCODER1_CH1_C3,
    .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C25,
};

MotorPID motor_left_pid = {
    .KP = 0,            .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};
MotorPID motor_right_pid = {
    .KP = 0,            .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};
MotorPID motor_front_pid = {
    .KP = 0,            .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};

// TODO
Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
    {
        .index = LEFT,              .pwm_channel_forward = PWM2_MODULE0_CHA_C6, .pwm_channel_backward = PWM2_MODULE0_CHB_C7,
        .pwm_duty = 0,              .current_speed = 0,                         .set_speed = 0,
        .encoder = &encoder_left,   .PID = &motor_left_pid,
    },
    {
        .index = RIGHT,             .pwm_channel_forward = PWM2_MODULE1_CHA_C8, .pwm_channel_backward = PWM2_MODULE1_CHB_C9,
        .pwm_duty = 0,              .current_speed = 0,                         .set_speed = 0,
        .encoder = &encoder_right,  .PID = &motor_right_pid,
    },
    {
        .index = FRONT,             .pwm_channel_forward = PWM2_MODULE2_CHA_C10, .pwm_channel_backward = PWM2_MODULE2_CHB_C11,
        .pwm_duty = 0,              .current_speed = 0,                          .set_speed = 0,
        .encoder = &encoder_front,  .PID = &motor_front_pid,
    },
};

// 启动所有电机PWM通道输出，占空比为0
void motor_all_init(void){
    for(int i = 0; i < MOTOR_INDEX_MAX_PLUS_ONE; i++){
        pwm_init(motors[i].pwm_channel_forward,     MOTOR_PWM_FREQUENCY, 0);
        pwm_init(motors[i].pwm_channel_backward,    MOTOR_PWM_FREQUENCY, 0);
    }
}

// 设置电机PWM通道输出占空比
void motor_set_duty(MotorIndex index, int32 duty){
    if(abs(duty) > PWM_DUTY_MAX){
        (duty > 0) ? (duty = PWM_DUTY_MAX) : (duty = -PWM_DUTY_MAX);
    }
    if(duty > 0){
        pwm_set_duty(motors[index].pwm_channel_backward,    0);
        pwm_set_duty(motors[index].pwm_channel_forward,     duty);
    }else if(duty < 0){
        pwm_set_duty(motors[index].pwm_channel_backward,    -duty);
        pwm_set_duty(motors[index].pwm_channel_forward,     0);
    }else if(duty == 0){
        pwm_set_duty(motors[index].pwm_channel_backward,    0);
        pwm_set_duty(motors[index].pwm_channel_forward,     0);
    }else{
        // TODO 
        // Wrong!!
    }
    // 记录占空比
    motors[index].pwm_duty = duty;
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
    // 比例
    motor->PID->wrong = motor->set_speed - motor->current_speed;
    out += (double) (motor->PID->KP * motor->PID->wrong);

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
    motor_set_duty(index, (int32)out);
}
void motor_pid_pit_call(void){
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
        motor_pid_calc_apply(index);
    }
}

// Motor encoder
void encoder_all_init(){
    encoder_quad_init(encoder_left.encoder_index,encoder_left.encoder_channel_1,encoder_left.encoder_channel_2);
    encoder_quad_init(encoder_right.encoder_index,encoder_right.encoder_channel_1,encoder_right.encoder_channel_2);
    encoder_quad_init(encoder_front.encoder_index,encoder_front.encoder_channel_1,encoder_front.encoder_channel_2);
}
void motor_encoder_pit_init(void){
    pit_init(MOTOR_ENCODER_PIT,MOTOR_ENCODER_PIT_TIME);
}
void motor_encoder_pit_call(void){
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
        // 获取速度
        motors[index].current_speed = encoder_get_speed(motors[index].encoder->encoder_index);
        // 清除计数
        encoder_clear_count(motors[index].encoder->encoder_index);
    }
}
