#ifndef _CODE_MOTOR_H_
#define _CODE_MOTOR_H_

#include "zf_driver_pwm.h"

#define MOTOR_PWM_FREQUENCY 50
#define MOTOR_PWM_DUTY_MAX  10000

typedef enum _MotorIndex{
    LEFT,   RIGHT,  FRONT,  MOTOR_INDEX_MAX_PLUS_ONE,
} MotorIndex;

typedef struct _Motor
{
    const MotorIndex              index;
    const pwm_channel_enum        pwm_channel_forward;
    const pwm_channel_enum        pwm_channel_backward;
          int32                   pwm_duty;
          int32                   current_speed;
          int32                   set_speed;
} Motor;

void motor_all_init(void){
    
}
void motor_set_duty(MotorIndex index, int32 duty);
void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_unpower(MotorIndex index);
void motor_all_stop(void);

#endif