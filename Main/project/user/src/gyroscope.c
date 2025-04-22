#include "gyroscope.h"
#include "math.h"

#define M_PI 3.141593f

vuint8 gyroscope_offset_test_finish = 0; // 陀螺仪偏置测试完成标志位
vint32 gyroscope_offset_test_count = 0;
volatile float gyro_z_offset = 0.;

GyroscopeResult gyroscope_result = {0}; // 陀螺仪积分数据

void gyroscope_init(void){

    // 初始化icm42688
    icm42688_init(&ICM42688_CONFIG);

}


void gyroscope_data_process(void) {
    gyroscope_result.gyro_z = ICM42688.gyro_z - gyro_z_offset; // 陀螺仪数据

    gyroscope_result.angle_z += (gyroscope_result.gyro_z) * GYROSCOPE_PIT_TIME / 1000.0f;
}

void gyroscope_pit_init(void){
    pit_ms_init(GYROSCOPE_PIT,GYROSCOPE_PIT_TIME);
}
void gyroscope_pit_callback(void){
    ICM42688_Get_Data();
    if(!gyroscope_offset_test_finish){
        if(gyroscope_offset_test_count < 1000){
            gyro_z_offset += ICM42688.gyro_z;
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
