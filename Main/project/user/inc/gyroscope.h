#ifndef _CODE_GYROSCOPE_H_
#define _CODE_GYROSCOPE_H_

#include "stdlib.h"

#include "icm42688.h"
#include "zf_driver_pit.h"

#define GYROSCOPE_PIT PIT_CH0
#define GYROSCOPE_PIT_TIME 1

typedef struct _GyroscopeResult{
    volatile float gyro_z;
    volatile float angle_z;
}GyroscopeResult;

extern GyroscopeResult gyroscope_result;
extern volatile float gyro_z_offset;

void  gyroscope_init(void);
void  gyroscope_data_process(void);
void  gyroscope_pit_init(void);
void  gyroscope_pit_callback(void);
void  gyroscope_filter(void);

#endif