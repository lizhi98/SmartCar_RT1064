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
Encoder encoder_rear = {
    .encoder_index      = QTIMER2_ENCODER1,
    .encoder_channel_1  = QTIMER2_ENCODER1_CH1_C3,
    .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C4,
};

MotorPID motor_left_pid = {
    .KP = 1.0,          .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};
MotorPID motor_right_pid = {
    .KP = 1.1,          .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};
MotorPID motor_rear_pid = {
    .KP = 1.0,          .KI = 0,        .KD = 0,
    .last_wrong = 0,    .sum_wrong = 0, .wrong = 0,
};

// TODO
Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
    {
        .index = LEFT,              .pwm_channel_pin = PWM2_MODULE0_CHB_C7,  .gpio_dir_pin = C6,
        .pwm_duty = 0,              .current_speed = 0,                      .set_speed = 0,
        .encoder = &encoder_left,   .PID = &motor_left_pid,
    },
    {
        .index = RIGHT,             .pwm_channel_pin = PWM2_MODULE2_CHB_C11, .gpio_dir_pin = C10,
        .pwm_duty = 0,              .current_speed = 0,                      .set_speed = 0,
        .encoder = &encoder_right,  .PID = &motor_right_pid,
    },
    {
        .index = REAR,              .pwm_channel_pin = PWM2_MODULE3_CHB_D3,  .gpio_dir_pin = D2,
        .pwm_duty = 0,              .current_speed = 0,                      .set_speed = 0,
        .encoder = &encoder_rear,   .PID = &motor_rear_pid,
    },
};

// TARGET MOTION
int32       target_speed_magnitude;
double      target_angle;
double      w_kp = -10;

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
    pwm_set_duty(motors[index].pwm_channel_pin,    abs(duty));
    if(duty > 0){
        gpio_set_level(motors[index].gpio_dir_pin, 1);
    }else if(duty < 0){
        gpio_set_level(motors[index].gpio_dir_pin, 0);
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
    encoder_quad_init(encoder_rear.encoder_index,encoder_rear.encoder_channel_1,encoder_rear.encoder_channel_2);
}
void motor_encoder_pit_init(void){
    pit_ms_init(MOTOR_ENCODER_PIT,MOTOR_ENCODER_PIT_TIME);
}
void motor_encoder_pit_call(void){
    for(MotorIndex index = 0;index < REAR;index ++){
        // 获取速度
        motors[index].current_speed = encoder_get_speed(motors[index].encoder->encoder_index);
        // 清除计数
        encoder_clear_count(motors[index].encoder->encoder_index);
    }
    // 获取速度
    motors[REAR].current_speed = encoder_get_speed(motors[REAR].encoder->encoder_index) / -4;
    // 清除计数
    encoder_clear_count(motors[REAR].encoder->encoder_index);
}

// 车运动解算函数
void target_motion_calc(void){
    // 目标速度正交分解
    // V前 = V要 * cos(目标角度弧度)
    // 有正负，分别代表向前和向后
    // double speed_front  =        target_speed_magnitude * cos(target_angle);
    // // V前 = V要 * sin(目标角度弧度)
    // // 有正负，分别代表向左和向右
    // double speed_left   =   -1 * target_speed_magnitude * sin(target_angle);

    double speed_front  =        target_speed_magnitude;
    double speed_left   =   0;
    // 计算各个轮子的移动速度
    
    int32 motor_left_speed,motor_right_speed,motor_rear_speed;
    
    // 平移运动
    motor_left_speed  = (int32) (speed_left * COS_PI_D_3 - speed_front * COS_PI_D_6);
    motor_right_speed = (int32) (speed_left * COS_PI_D_3 + speed_front * COS_PI_D_6);
    motor_rear_speed  = (int32) (speed_left * -1);
    // 自转运动 逆时针为正
    // motor_left_speed  += (int32) (target_angle * w_kp);
    // motor_right_speed += (int32) (target_angle * w_kp);
    // motor_rear_speed  += (int32) (target_angle * w_kp);

    motor_left_speed  += (int32) (w_kp * search_result.offset);
    motor_right_speed += (int32) (w_kp * search_result.offset);
    motor_rear_speed  += (int32) (w_kp * search_result.offset);

    // 应用速度
    motor_run_with_speed(LEFT,motor_left_speed);
    motor_run_with_speed(RIGHT,motor_right_speed);
    motor_run_with_speed(REAR,motor_rear_speed);
}
