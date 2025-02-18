#include "motor.h"

// TODO
Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
    {.index = LEFT,  .pwm_duty = 0, .current_speed = 0, .set_speed = 0},
    {.index = RIGHT, .pwm_duty = 0, .current_speed = 0, .set_speed = 0},
    {.index = FRONT, .pwm_duty = 0, .current_speed = 0, .set_speed = 0},
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

void motor_pid_init(void){
    pit_ms_init(MOTOR_PID_PIT,MOTOR_PID_PIT_TIME);
}
void motor_pid_calc_apply(MotorIndex index){
    double out = 0.;
    Motor * motor = &motors[index];
    // 比例
    motor->PID.wrong = motor->set_speed - motor->current_speed;
    out += (double) (motor->PID.KP * motor->PID.wrong);

    // 积分
    motor->PID.sum_wrong += motor->PID.wrong;
    // 积分限幅
    if(abs(motor->PID.sum_wrong) > SUM_WRONG_MAX){
        (motor->PID.sum_wrong > 0) ? (motor->PID.sum_wrong = SUM_WRONG_MAX) : (motor->PID.sum_wrong = -SUM_WRONG_MAX);
    }
    out += (double) (motor->PID.KI * motor->PID.sum_wrong);

    // 微分
    out += (double) (motor->PID.KD * (motor->PID.wrong - motor->PID.last_wrong));

    // 记录wrong
    motor->PID.last_wrong = motor->PID.wrong;

    // 输出
    motor_set_duty(index, (int32)out);
}
void motor_pid_timer_call(void){
    for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
        motor_pid_calc_apply(index);
    }
}
