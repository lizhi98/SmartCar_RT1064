#ifndef _CODE_MOTOR_H_
#define _CODE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_driver_pit.h"
#include "encoder.h"

#define MOTOR_PWM_FREQUENCY     50
#define MOTOR_PWM_DUTY_MAX      10000

#define SUM_WRONG_MAX           1000 		//TODO
#define MOTOR_PID_PIT           PIT_CH0	// TODO 需要根据实际修改
#define MOTOR_PID_PIT_TIME      20 			// TODO

#define MOTOR_ENCODER_PIT       PIT_CH1
#define MOTOR_ENCODER_PIT_TIME  20

typedef enum _MotorIndex{
    LEFT,   RIGHT,  FRONT,  MOTOR_INDEX_MAX_PLUS_ONE,
} MotorIndex;

typedef struct _MotorPID
{
    const float KP;
    const float KI;
    const float KD;
          float wrong;
          float last_wrong;
          float sum_wrong;
}MotorPID;

typedef struct _Motor
{
    const MotorIndex              index;
    const pwm_channel_enum        pwm_channel_forward;
    const pwm_channel_enum        pwm_channel_backward;
    const Encoder*                encoder;
          int32                   pwm_duty;
          int32                   current_speed;
          int32                   set_speed;
          MotorPID*               PID;
} Motor;

// Motor PWM Control
void motor_all_init(void);
void motor_set_duty(MotorIndex index, int32 duty);
void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_unpower(MotorIndex index);
void motor_all_stop(void);

// Motor PID Control
void motor_pid_pit_init(void);
void motor_pid_calc_apply(MotorIndex index);
void motor_pid_pit_call(void);

// Motor encoder
void motor_encoder_pit_init(void);
void motor_encoder_pit_call(void);

#endif
