#ifndef _CODE_MOTION_CONTROL_NEW_H
#define _CODE_MOTION_CONTROL_NEW_H

#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"
#include "math.h"
#include "main.h"
#include "mt_image.h"
#include "MCX_Vision.h"
#include "OpenART.h"
#include "pid_control.h"
#include "menu.h"
#include "gyroscope.h"
#include "grayscale.h"

#define MOTOR_H_PI      3.14159
#define COS_PI_D_6      0.86603
#define SIN_PI_D_6      0.5
#define COS_PI_D_3      0.5
#define SIN_PI_D_3      0.86603

// MOTOR PWM
#define MOTOR_PWM_FREQUENCY         17000
#define MOTOR_PWM_DUTY_MAX          4000

// ENCODER
#define MOTOR_ENCODER_PIT           PIT_CH2
#define MOTOR_ENCODER_PIT_TIME      2

#define MOTOR_PID_PIT               PIT_CH1
#define MOTOR_PID_PIT_TIME          2

#define IMAGE_IDENTIFY_WAIT_TIME    3000 // 立方体识别等待时间，单位ms

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

typedef struct _Motor
{
             MotorIndex          index;
             pwm_channel_enum    pwm_channel_pin;
             gpio_pin_enum       gpio_dir_pin;
             Encoder *           encoder;
    volatile int32               pwm_duty;
    volatile int32               current_speed;
    volatile int32               set_speed;
             PID *               pid_controller;
} Motor;

// MOTION MODE
typedef enum _MotionMode{
    LINE_FOLLOW,
    CUBE_DISTANCE_POSITION,
    CUBE_IDENTIFY,
    CUBE_ANGLE_POSITION,
    CUBE_PUSH,
    LINE_POSITION_BACK,
    LINE_ANGLE_BACK,
}MotionMode;

typedef struct _MotionControl
{
    volatile MotionMode  motion_mode;
    volatile MotionMode  last_motion_mode;
    volatile float       line_gyro_angle_z;
    volatile float       target_gyro_angle_z; // 目标陀螺仪角度
}MotionControl;

// 编码器获取值滤波
#define WINDOW_SIZE 5  // 滑动窗口大小
// 移动平均滤波
typedef struct _MovingAverageFilter{
    int16 buffer[WINDOW_SIZE];
    uint8_t index;
    int16 sum;
} MovingAverageFilter;

extern MotionControl motion_control;
extern Motor motors[MOTOR_INDEX_MAX_PLUS_ONE];

extern uint8 motor_rotation_translation_pid_calc_flag;
extern uint8 motor_translation_angular_v_pid_calc_flag;
extern uint8 motor_speed_pid_calc_flag;

extern CubePushDir cube_push_dir;

extern uint8 motor_enable_flag; // 电机使能标志位

// Motor PWM Control
void motor_set_duty(MotorIndex index, int32 duty);
void motor_plus_duty(MotorIndex index, int32 delta_duty);

void motor_run_with_speed(MotorIndex index, int32 speed);
void motor_all_unpower(void);
void motor_all_stop(void);
void motor_all_init(void);

// Motor encoder
void motor_encoder_pit_init(void);
void motor_encoder_pit_callback(void);
void encoder_all_init(void);

#define encoder_get_speed(encoder_index) encoder_get_count(encoder_index)

void motion_control_pid_callback(void);

void motion_control_pid_pit_init(void);

void motion_mode_calc();

#endif
