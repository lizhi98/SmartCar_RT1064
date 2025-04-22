#ifndef _CODE_MOTOR_H_
#define _CODE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"
#include "math.h"
#include "mt_image.h"
#include "MCX_Vision.h"
#include "OpenMV.h"

#define MOTOR_H_PI      3.14159
#define COS_PI_D_6      0.86603
#define SIN_PI_D_6      0.5
#define COS_PI_D_3      0.5
#define SIN_PI_D_3      0.86603

// MOTOR PWM
#define MOTOR_PWM_FREQUENCY         17000
#define MOTOR_PWM_DUTY_MAX          5000

// SPEED PID
#define SUM_WRONG_MAX               5000
#define MOTOR_PID_PIT               PIT_CH2
#define MOTOR_PID_PIT_TIME          20

// ENCODER
#define MOTOR_ENCODER_PIT           PIT_CH2
#define MOTOR_ENCODER_PIT_TIME      20

// MOTION PID
#define MOTION_PID_PIT              PIT_CH1
#define MOTION_PID_PIT_TIME         10

#define ROTATION_SUM_OFFSET_MAX     400
#define TRANSLATION_SUM_OFFSET_MAX  40

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

typedef struct _RotationPID{
    volatile float KP;
    volatile float KI;
    volatile float KD;
    volatile int32 offset;
    volatile int32 last_offset;
    volatile int32 sum_offset;
    volatile int32 wl_out;
} RotationPID;

typedef struct _TranslationPID{
             float front_KP;
             float front_KI;
             float front_KD;
    volatile int32 front_offset;
    volatile int32 front_last_offset;
    volatile int32 front_sum_offset;
    volatile int32 front_speed_out;

             float left_KP;
             float left_KI;
             float left_KD;
    volatile int32 left_offset;
    volatile int32 left_last_offset;
    volatile int32 left_sum_offset;
    volatile int32 left_speed_out;
} TranslationPID;

typedef struct _MotorPID
{
    volatile float KP;
    volatile float KI;
    volatile float KD;
    volatile int32 wrong;
    volatile int32 last_wrong;
    volatile int32 sum_wrong;
} MotorPID;

typedef struct _Motor
{
             MotorIndex          index;
             pwm_channel_enum    pwm_channel_pin;
             gpio_pin_enum       gpio_dir_pin;
             Encoder *           encoder;
    volatile int32               pwm_duty;
    volatile int32               current_speed;
    volatile int32               set_speed;
             MotorPID *          PID;
} Motor;

// MOTION MODE
typedef enum _MotionMode{
    LINE_FOLLOW,
    CUBE_DISTANCE_POSITION,
    CUBE_IDENTIFY,
    CUBE_ANGLE_POSITION_LEFT,
    CUBE_ANGLE_POSITION_RIGHT,
    CUBE_PUSH,
    LINE_BACK,
}MotionMode;

typedef struct _MotionControl
{
    volatile MotionMode  motion_mode;
    volatile MotionMode  last_motion_mode;
    volatile float       line_gyro_angle_z;
             vint32      line_distance; // 当前距离赛道内部的距离
}MotionControl;

// MOTOR
extern Motor motors[MOTOR_INDEX_MAX_PLUS_ONE];
// PID
extern RotationPID      rotation_pid;
extern TranslationPID   translation_pid;

// TARGET MOTION
extern MotionControl motion_control;

// Motor PWM Control
void motor_set_duty(MotorIndex index, int32 duty);
void motor_plus_duty(MotorIndex index, int32 delta_duty);

void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_unpower(MotorIndex index);
void motor_all_stop(void);
void motor_all_init(void);

// Motor PID Control
void motor_pid_pit_init(void);
void motor_pid_calc_apply(MotorIndex index);
void motor_pid_pit_call(void);

// MOTION PID Control
void motion_pid_pit_init(void);
void motion_pid_callback(void);
void rotation_pid_calc(void);
void translation_pid_calc(void);

// Motor encoder
void motor_encoder_pit_init(void);
void motor_encoder_pit_call(void);
void encoder_all_init(void);

#define encoder_get_speed(encoder_index) encoder_get_count(encoder_index)

// 车运动解算函数
void target_motion_calc(void);

#endif
