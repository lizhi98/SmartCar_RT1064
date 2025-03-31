#include "gyroscope.h"

gyroscope_integrated_data gyroscope_processed_data = {0}; // 陀螺仪积分数据

void gyroscope_init(void){

    // 配置初始化结构体
    icm42688_init(&ICM42688_CONFIG);

}

void gyroscope_data_process(void){
    // 积分数据
    gyroscope_processed_data.x  += (double)(ICM42688.gyro_x * GYROSCOPE_PIT_TIME / 1000.0f);
    gyroscope_processed_data.y  += (double)(ICM42688.gyro_y * GYROSCOPE_PIT_TIME / 1000.0f);
    gyroscope_processed_data.z  += (double)(ICM42688.gyro_z * GYROSCOPE_PIT_TIME / 1000.0f);
    gyroscope_processed_data.vx += (double)(ICM42688.acc_x * GYROSCOPE_PIT_TIME / 1000.0f);
    gyroscope_processed_data.vy += (double)(ICM42688.acc_y * GYROSCOPE_PIT_TIME / 1000.0f);
    gyroscope_processed_data.vz += (double)(ICM42688.acc_z * GYROSCOPE_PIT_TIME / 1000.0f);
}

void gyroscope_pit_init(void){
    pit_ms_init(GYROSCOPE_PIT,GYROSCOPE_PIT_TIME);
}
void gyroscope_pit_call(void){
    ICM42688_Get_Data();
    gyroscope_data_process();
}
