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
    system_delay_ms(500);
    screen_init();         // 屏幕初始化
    mt_camera_init();       // 摄像头初始化
    ips200_show_string(0, 0, "Koishi SmartCar");
    // correspond_host_cmd_init();  // 上位机命令接收初始化
    encoder_all_init();       // 编码器初始化
    motor_encoder_pit_init(); // 编码器PIT定时器初始化
    motor_all_init();         // 电机初始化
    motion_control_pid_pit_init(); // PID定时器初始化
    // correspond_image_send_init(); // 图像发送初始化
    mcx_init_wait(); // MCX_Vision 初始化
    mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
    mcx_uart_data_handle_pit_init();
    ips200_show_string(0, 20, "Motor Init OK");
    
    uint32 i = 0;
    motion_control.motion_mode = LINE_FOLLOW; // 设置初始运动模式为巡线
    while (1)
    {
        if(motion_control.motion_mode == LINE_FOLLOW){
            if(mt9v03x_finish_flag){
                process_image(mt9v03x_image); // 处理图像
                
                // seekfree_assistant_camera_send(); // 发送图像到上位机
                mt9v03x_finish_flag = 0; // 清除图像采集完成标志位
            }
        }
        motion_mode_calc();
        // ips200_show_int(0, 40, i++, 5); // 显示计数
        // correspond_send_info_to_host(); // 发送信息到上位机
    }    
}
