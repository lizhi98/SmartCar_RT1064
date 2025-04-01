#ifndef _CODE_GYROSCOPE_H_
#define _CODE_GYROSCOPE_H_

#include "icm42688.h"
#include "zf_driver_pit.h"

#define GYROSCOPE_PIT PIT_CH0
#define GYROSCOPE_PIT_TIME 5

typedef struct _gyroscope_integrated_data{
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;
}gyroscope_integrated_data;

extern gyroscope_integrated_data gyroscope_processed_data;

void  gyroscope_init(void);
void  gyroscope_data_process(void);
void  gyroscope_pit_init(void);
void  gyroscope_pit_call(void);

#endif