#include "gyroscope.h"
#include "math.h"

#define M_PI 3.141593f

vuint8 gyroscope_offset_test_finish = 0; // 陀螺仪偏置测试完成标志位
vint32 gyroscope_offset_test_count = 0;
volatile float gyro_z_offset = 0.;

GyroscopeResult gyroscope_result = {0}; // 陀螺仪积分数据

void gyroscope_init(void){

    // 初始化icm42688
    // icm42688_init(&ICM42688_CONFIG);
    imu660ra_init();

}


void gyroscope_data_process(void) {
    gyroscope_result.gyro_z = imu660ra_gyro_transition(imu660ra_gyro_z) - gyro_z_offset; // 陀螺仪数据

    gyroscope_result.angle_z += (gyroscope_result.gyro_z) * GYROSCOPE_PIT_TIME / 1000.0f;
}

void gyroscope_pit_init(void){
    pit_ms_init(GYROSCOPE_PIT,GYROSCOPE_PIT_TIME);
}
void gyroscope_pit_callback(void){
    imu660ra_get_gyro();
    if(!gyroscope_offset_test_finish){
        if(gyroscope_offset_test_count < 1000){
            gyro_z_offset += imu660ra_gyro_transition(imu660ra_gyro_z);
            gyroscope_offset_test_count++;
        }
        if(gyroscope_offset_test_count == 1000){
            gyro_z_offset /= 1000;
            gyroscope_offset_test_count = 0;
            gyroscope_offset_test_finish = 1;
        }
    }else{
        gyroscope_data_process();
    }
}
