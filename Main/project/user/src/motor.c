#include "motor.h"

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
    
}
void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_unpower(MotorIndex index);
void motor_all_stop(void);
