#include "gyroscope.h"

// ====================卡尔曼滤波器====================
//以高度为例 定义卡尔曼结构体并初始化参数
// KFP KFP_x = {0.02,0,0,0,0.001,0.543};

Kalman_pm_st    kalman_pm;
gyroscope_data  gyro_data;

void kalman_init(void)
{
/*注意：只需要调节Q矩阵[Q_angle,Q_gyro]和R矩阵[R_angle]即可*/
 
    kalman_pm.P[0][0] = 1.0f;
    kalman_pm.P[0][1] = 0.0f;
    kalman_pm.P[1][0] = 0.0f;
    kalman_pm.P[1][1] = 1.0f;
 
    kalman_pm.dt = 0.001f;
    kalman_pm.Q_angle = 0.001f;
    kalman_pm.Q_gyro = 0.003f;
    kalman_pm.R_angle = 0.5f;
    kalman_pm.q_bias = 0.0f;
    kalman_pm.angle_f = 0.0f;
}

/**
 *卡尔曼滤波器
 *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *滤波后的参数（最优值）
 */
// float kalman_filter(KFP *kfp,float input){
//      //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
//      kfp->Now_P = kfp->LastP + kfp->Q;
//      //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
//      kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
//      //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
//      kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//因为这一次的预测值就是上一次的输出值
//      //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
//      kfp->LastP = (1-kfp->Kg) * kfp->Now_P;
//      return kfp->out;
// }

void kalman_filter(Kalman_pm_st *kalman_pm)
{
    float angle_err;  //先验误差
    float angle_;     //先验估计
    float Pdot[2][2]; //先验误差协方差矩阵
 
    float K_0;
    float K_1; //卡尔曼增益
 
    /*先验估计*/
    angle_ = kalman_pm->angle_f + (kalman_pm->wb_m - kalman_pm->q_bias) * kalman_pm->dt; //先验估计
    angle_err = kalman_pm->angle_m - angle_;                                             //先验误差
 
    /*先验误差协方差矩阵*/
    Pdot[0][0] = kalman_pm->P[0][0] + kalman_pm->Q_angle - (kalman_pm->P[0][1] + kalman_pm->P[1][0]) * kalman_pm->dt; // Q_angle->Q1
    Pdot[0][1] = kalman_pm->P[0][1] - (kalman_pm->P[1][1]) * kalman_pm->dt;
    Pdot[1][0] = kalman_pm->P[1][0] - (kalman_pm->P[1][1]) * kalman_pm->dt;
    Pdot[1][1] = kalman_pm->P[1][1] + kalman_pm->Q_gyro; // Q_gyro->Q2
 
    /*卡尔曼增益*/
    K_0 = Pdot[0][0] / (Pdot[0][0] + kalman_pm->R_angle);
    K_1 = Pdot[1][0] / (Pdot[0][0] + kalman_pm->R_angle);
 
    /*后验估计*/
    kalman_pm->angle_f = angle_ + K_0 * angle_err;         //最优角度
    kalman_pm->q_bias += K_1 * angle_err;                  //最优角速度偏差
    kalman_pm->wb_f = kalman_pm->wb_m - kalman_pm->q_bias; //最优角速度
 
    /*更新误差协方差矩阵*/
    kalman_pm->P[0][0] = Pdot[0][0] - K_0 * Pdot[0][0];
    kalman_pm->P[0][1] = Pdot[0][1] - K_0 * Pdot[0][1];
    kalman_pm->P[1][0] = Pdot[1][0] - K_1 * Pdot[0][0];
    kalman_pm->P[1][1] = Pdot[1][1] - K_1 * Pdot[0][1];
}

void gyroscope_init(void){
    icm42688_init(&ICM42688_CONFIG);
    // 滤波准备
    kalman_init();
}

void gyroscope_pit_init(void){
    pit_ms_init(GYROSCOPE_PIT,GYROSCOPE_PIT_TIME);
}
void gyroscope_pit_call(void){
    ICM42688_Get_Data();
    
    kalman_pm.angle_m   =   ICM42688.acc_x;      //加速度计求解角度的值(偏航角)
    kalman_pm.wb_m      =   ICM42688.gyro_x;     //陀螺仪的角速度(偏航角速度)
    kalman_filter(&kalman_pm);
    float x_angle       =   kalman_pm.angle_f;   //滤波后的角度
    float x_w           =   kalman_pm.wb_f;      //滤波后的角速度
}
