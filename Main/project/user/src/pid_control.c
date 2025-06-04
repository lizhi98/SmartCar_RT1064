#include "pid_control.h"

void PID_set(PIDType type, PID *pid, float kp, float ki, float kd){
    pid->type   = type;
    pid->kp     = kp;
    pid->ki     = ki;
    pid->kd     = kd;
}

void PID_calculate(PID *pid, float target, float current){
    pid->error = target - current;  // 计算误差

    switch(pid->type) {
        case PID_POI: // 位置PID
            pid->error_sum += pid->error;   // 累加误差
            // 累积误差清零
            if(pid->error == 0.0f) {
                pid->error_zero_count++;
                if (pid->error_zero_count > 50) { // 连续误差为0超过50次，清零误差累加值
                    pid->error_sum = 0.0f;
                    pid->error_zero_count = 0;
                }
            } else {
                pid->error_zero_count = 0; // 重置误差为0计数
            }
            // 积分限幅
            if (pid->error_sum > pid->error_limit) {
                pid->error_sum = pid->error_limit; // 限制误差累加值
            } else if (pid->error_sum < -pid->error_limit) {
                pid->error_sum = -pid->error_limit; // 限制误差累加值
            }

            float delta_error = pid->error - pid->pre_error; // 计算误差变化量
            pid->output = pid->kp * pid->error + pid->ki * pid->error_sum + pid->kd * delta_error; // PID公式
            pid->pre_error = pid->error; // 更新前一次误差
            break;
        case PID_INC: // 增量PID
            if(pid->error == 0.0f) {
                pid->error_zero_count++;
            } else {
                pid->error_zero_count = 0; // 重置误差为0计数
            }
            pid->output = pid->kp * (pid->error - pid->pre_error) + 
                          pid->ki * pid->error + 
                          pid->kd * (pid->error - 2 * pid->pre_error + pid->pre_pre_error); // 增量PID公式
            pid->pre_pre_error = pid->pre_error; // 更新前前一次误差
            pid->pre_error = pid->error; // 更新前一次误差
            break;
        default:
            pid->output = 0.0f; // 未知类型，输出为0
            break;
    }
    
}

void PID_clean(PID *pid){
    pid->pre_pre_error = 0.0f;
    pid->pre_error = 0.0f;
    pid->error = 0.0f;
    pid->error_sum = 0.0f;
    pid->error_zero_count = 0;
    pid->output = 0.0f;
}
