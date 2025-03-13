#ifndef _CODE_MOTOR_H_
#define _CODE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"
#include "math.h"

#define MOTOR_H_PI      3.14159
#define COS_PI_D_6      0.86603
#define SIN_PI_D_6      0.5
#define COS_PI_D_3      0.5
#define SIN_PI_D_3      0.86603

// MOTOR PWM
#define MOTOR_PWM_FREQUENCY     17000
#define MOTOR_PWM_DUTY_MAX      4000

// PID
#define SUM_WRONG_MAX           4000    // TODO
#define MOTOR_PID_PIT           PIT_CH0 // TODO 需要根据实际修改
#define MOTOR_PID_PIT_TIME      20 // TODO

// ENCODER
#define MOTOR_ENCODER_PIT       PIT_CH0
#define MOTOR_ENCODER_PIT_TIME  20

typedef enum _MotorIndex
{
    LEFT,
    RIGHT,
    REAR,
    MOTOR_INDEX_MAX_PLUS_ONE,
} MotorIndex;

typedef struct _Encoder
{
    encoder_index_enum      encoder_index;
    encoder_channel1_enum   encoder_channel_1;
    encoder_channel2_enum   encoder_channel_2;
} Encoder;

typedef struct _MotorPID
{
    float KP;
    float KI;
    float KD;
    float wrong;
    float last_wrong;
    float sum_wrong;
} MotorPID;

typedef struct _Motor
{
    MotorIndex          index;
    pwm_channel_enum    pwm_channel_pin;
    gpio_pin_enum       gpio_dir_pin;
    Encoder *           encoder;
    int32               pwm_duty;
    int32               current_speed;
    int32               set_speed;
    MotorPID *          PID;
} Motor;

// MOTOR
extern Motor motors[MOTOR_INDEX_MAX_PLUS_ONE];

// TARGET MOTION
extern int32       target_speed_magnitude;
extern double      target_angle;
extern double      speed_kp;

// Motor PWM Control
void motor_set_duty(MotorIndex index, int32 duty);
void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_unpower(MotorIndex index);
void motor_all_stop(void);
void motor_all_init(void);

// Motor PID Control
void motor_pid_pit_init(void);
void motor_pid_calc_apply(MotorIndex index);
void motor_pid_pit_call(void);

// Motor encoder
void motor_encoder_pit_init(void);
void motor_encoder_pit_call(void);
void encoder_all_init(void);

#define encoder_get_speed(encoder_index) encoder_get_count(encoder_index)

// 车运动解算函数
void target_motion_calc(void);

#endif
