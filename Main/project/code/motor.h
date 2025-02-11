#ifndef _CODE_MOTOR_H_
#define _CODE_MOTOR_H_

#include "zf_driver_pwm.h"

typedef struct _Motor
{
    pwm_channel_enum pwm_channel_forward;
    pwm_channel_enum pwm_channel_backward;
    const uint32 freq;
    const uint32 duty_max;
    int32 duty;
    int32 current_speed;
    int32 set_speed;
    void (*init)    (struct _Motor  *this_motor);
    void (*unpower) (struct _Motor  *this_motor);
    
    
} Motor;


#endif