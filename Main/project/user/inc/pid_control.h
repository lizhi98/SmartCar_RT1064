#ifndef _CODE_PID_CONTROL_H
#define _CODE_PID_CONTROL_H

#include "zf_common_typedef.h"

typedef enum _PIDType{
   PID_POI,
   PID_INC,
} PIDType;

typedef struct _PID{
   PIDType  type; // PID类型
   float    kp;
   float    ki;
   float    kd;
   float    pre_pre_error;
   float    pre_error;
   float    error;
   float    error_sum;
   float    error_limit; // 误差限制
   uint32   error_zero_count; // 误差为0的计数
   float    output;
} PID;

void PID_set(PIDType type, PID *pid, float kp, float ki, float kd);

void PID_calculate(PID *pid, float target, float current);

void PID_clean(PID *pid);

#endif