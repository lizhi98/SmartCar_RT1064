#ifndef _CODE_GYROSCOPE_H_
#define _CODE_GYROSCOPE_H_

#include "icm42688.h"
#include "zf_driver_pit.h"

#define GYROSCOPE_PIT PIT_CH2
#define GYROSCOPE_PIT_TIME 50

// //结构体类型定义
// typedef struct 
// {
//     float LastP;//上次估算协方差 初始化值为0.02
//     float Now_P;//当前估算协方差 初始化值为0
//     float out;//卡尔曼滤波器输出 初始化值为0
//     float Kg;//卡尔曼增益 初始化值为0
//     float Q;//过程噪声协方差 初始化值为0.001
//     float R;//观测噪声协方差 初始化值为0.543
// }KFP;//Kalman Filter parameter

typedef struct _Kalman_pm_st
{
    float dt;      //采样时间
    float angle_f; //角度滤波后
    float angle_m; //角度测量
    float wb_m;    //角速度测量
    float wb_f;    //角速度滤波后
    float q_bias;  //角速度offset
    float P[2][2]; //协方差矩阵
 
    float Q_angle; // Q矩阵
    float Q_gyro;
 
    float R_angle; // R矩阵
} Kalman_pm_st;

typedef struct _gyroscope_data{
    float gyro_x;
    float acc_x;
}gyroscope_data;

extern Kalman_pm_st kalman_pm;
extern gyroscope_data gyro_data;

// float kalman_filter(KFP *kfp,float input);
void  gyroscope_init(void);
void kalman_filter(Kalman_pm_st *kalman_pm);
void  gyroscope_pit_init(void);
void  gyroscope_pit_call(void);

#endif