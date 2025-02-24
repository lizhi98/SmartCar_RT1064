#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程

#include "main.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化

    // =============外设初始化代码=============
    // 屏幕初始化
    // TODO
    // WIFI SPI 模块初始化
    if(wifi_spi_init_(10)){ // 超时时间10秒
        // TODO
    };
    // UART TO OpenART mini 初始化
    uart_init_();
    // mt9v034摄像头初始化
    if(mt_camera_init()){
        // 初始化失败 // TODO
    }
    // 电机初始化
    motor_all_init();
    // 编码器初始化
    encoder_all_init();
    // =============外设初始化代码=============
    while(1)
    {
        // 此处编写需要循环执行的代码
        
        
        
        // 此处编写需要循环执行的代码
    }
}



