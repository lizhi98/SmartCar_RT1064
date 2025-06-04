#include "zf_common_headfile.h"

#include "main.h"


// // 斑马线有效标志
uint8 zebra_valid_flag = 0;
// // 图像处理时间记录
uint32 image_process_time_start = 0;
uint32 image_process_time = 0;


int main(void)
{
    clock_init(SYSTEM_CLOCK_600M); 
    system_delay_ms(1000);
    screen_init();         // 屏幕初始化
    mt_camera_init();       // 摄像头初始化
    ips200_show_string(0, 0, "Koishi SmartCar");
    correspond_host_cmd_init();  // 上位机命令接收初始化
    encoder_all_init();       // 编码器初始化
    motor_encoder_pit_init(); // 编码器PIT定时器初始化
    motor_all_init();         // 电机初始化
    motion_control_pid_pit_init(); // PID定时器初始化
    // motor_set_duty(LEFT,  -2000);   // 左电机占空比设置为0
    // motor_set_duty(RIGHT, 2000);  // 右电机占空比设置为0
    // motor_set_duty(REAR, 2000);   // 后电机占空比设置为0
    ips200_show_string(0, 20, "Motor Init OK");
    uint32 i = 0;
    while (1)
    {
        if(mt9v03x_finish_flag){
            process_image(mt9v03x_image); // 处理图像
            mt9v03x_finish_flag = 0; // 清除图像采集完成标志位
        }
        // ips200_show_int(0, 40, i++, 5); // 显示计数
        correspond_send_info_to_host(); // 发送信息到上位机
    }    
}
