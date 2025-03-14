#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程
#include "main.h"
#include "zf_driver_pwm.h"
#include "zf_driver_gpio.h"
#include "icm42688.h"
#include "stdio.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化
    // =====================外设初始化======================
    // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
    uint8 hardware_init_flag = 0;
    // 屏幕初始化
    screen_init();
    // WIFI SPI 初始化
    // hardware_init_flag = wifi_spi_init_(20);
		hardware_init_flag = wifi_spi_init_();
    hardware_init_flag = wifi_spi_socket_connect("TCP","192.168.137.1","9894","6060");
    // UART TO OpenART mini 初始化
    // TODO
    // 摄像头初始化
    // hardware_init_flag = mt_camera_init();
    // 陀螺仪初始化
    
    // 电机初始化
    // 编码器初始化
    encoder_all_init();
    motor_encoder_pit_init();
    // 初始化完成，屏幕输出初始化结果
    if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
    else                  {screen_show_str(0,0,"HW Init Success");}
    system_delay_ms(2000);
    screen_clear();
    // =====================外设初始化======================
    // pwm_init(motors[0].pwm_channel_pin, MOTOR_PWM_FREQUENCY, 2000);
    // pwm_init(motors[1].pwm_channel_pin, MOTOR_PWM_FREQUENCY, 2000);
    // pwm_init(motors[2].pwm_channel_pin, MOTOR_PWM_FREQUENCY, 2000);
    // gpio_init(motors[0].gpio_dir_pin, GPO, 1, GPO_PUSH_PULL);
    // gpio_init(motors[1].gpio_dir_pin, GPO, 1, GPO_PUSH_PULL);
    // gpio_init(motors[2].gpio_dir_pin, GPO, 1, GPO_PUSH_PULL);
    char temp[30];
    while(1)
    {
        // ICM42688_Get_Data();
        system_delay_ms(200);
        sprintf(temp,"%d.0,%d.0,%d.0\n",motors[0].current_speed,motors[1].current_speed,motors[2].current_speed);
        wifi_spi_send_string(temp);
        // 此处编写需要循环执行的代码
    }
}



