#include "zf_common_headfile.h"

#include "main.h"

#define HOST_DEBUG 0

// 图像处理时间记录
uint32 image_process_time_start = 0;
uint32 image_process_time = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    // debug_init();                   // 调试端口初始化
    // =====================外设初始化开始=========================
    // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
    uint8 hardware_init_flag = 0;
    // 屏幕初始化
    screen_init();

    // UART TO OpenART mini 初始化
    // TODO
    // UART TO MCX_Vision 初始化
    hardware_init_flag += mcx_init_wait();
    mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
    pit_ms_init(PIT_CH3, 20); // MCX_Vision 定时器初始化
    // 摄像头初始化
    hardware_init_flag += mt_camera_init();
    // 陀螺仪初始化
    gyroscope_init();
    gyroscope_pit_init();

    // 电机初始化
    motor_all_init();
    // 编码器初始化
    encoder_all_init();
    motor_encoder_pit_init();
    rotation_pid_pit_init();    
#if HOST_DEBUG == 1
    // 上位机控制初始化
    hardware_init_flag += correspond_host_cmd_init();
    correspond_host_cmd_pit_init();
    // 图传初始化
    correspond_image_send_init();
#endif
    // 计时器初始化
    timer_init(GPT_TIM_1, TIMER_MS);
    timer_start(GPT_TIM_1);

    // 初始化完成，屏幕输出初始化结果
    if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
    else                  {screen_show_str(0,0,"HW Init Success");}

    system_delay_ms(2000);
    screen_clear();

    // =====================外设初始化结束========================

    // 主循环准备
    timer_init(GPT_TIM_1, TIMER_MS);
    timer_start(GPT_TIM_1);
    
    // target_speed_magnitude = 700;

    // 主循环
    while(1) {
        // 定时发送速度信息
        if (timer_get(GPT_TIM_1) > WIFI_SPI_SEND_INTERVAL) {
            // timer_clear(GPT_TIM_1);
#if HOST_DEBUG == 1
            correspond_send_info_to_host();
#endif
        }

        // 计算图像处理时间
        // 图像处理、发送与运动解算
        if (mt9v03x_finish_flag) {

            image_process_time_start = timer_get(GPT_TIM_1);
            // system_delay_ms(1000);
            
            process_image(mt9v03x_image);
            char buffer[32];
            sprintf(buffer,"%1d,%4d,%4d,%ld    ",cube_info.state,cube_info.x_offset,cube_info.y_offset,image_process_time);
            ips200_show_string(0, 0, buffer);
#if HOST_DEBUG == 1
        //seekfree_assistant_camera_send();
#endif
            image_process_time = timer_get(GPT_TIM_1) - image_process_time_start;
            mt9v03x_finish_flag = 0;
        }
        // 计算图像处理时间
        // 运动解算        
        target_motion_calc();

    }
}

