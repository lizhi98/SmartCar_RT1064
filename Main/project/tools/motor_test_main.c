// #include "zf_common_headfile.h"
// // 打开新的工程或者工程移动了位置务必执行以下操作
// // 第一步 关闭上面所有打开的文件
// // 第二步 project->clean  等待下方进度条走完
// // 本例程是开源库移植用空工程
// #include "main.h"

// // 图像处理时间记录
// uint32 image_process_time_start = 0;
// uint32 image_process_time = 0;

// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M);  // 不可删除
//     debug_init();                   // 调试端口初始化
//     // =====================外设初始化开始=========================
//     // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
//     uint8 hardware_init_flag = 0;
//     // 屏幕初始化
//     screen_init();

//     // UART TO OpenART mini 初始化
//     // TODO

//     // 摄像头初始化

//     // 电机初始化
//     motor_all_init();
//     // 编码器初始化
//     encoder_all_init();
//     motor_encoder_pit_init();
//     // 上位机控制初始化
//     hardware_init_flag += correspond_host_cmd_init();

//     correspond_host_cmd_pit_init();

//     // 图传初始化
//     // correspond_image_send_init();

//     // 计时器初始化
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);

//     // 初始化完成，屏幕输出初始化结果
//     if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
//     else                  {screen_show_str(0,0,"HW Init Success");}

//     system_delay_ms(2000);
//     screen_clear();
    
//     // =====================外设初始化结束========================

//     // 主循环准备
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);
//     // target_speed_magnitude = 800;

//     // motor_set_duty(LEFT, 2000);
//     motor_set_duty(RIGHT, 2000);
//     // motor_set_duty(REAR, 2000);
//     // 主循环
//     while(1) {
//         // 定时发送速度信息
//         if (timer_get(GPT_TIM_1) > WIFI_SPI_SEND_INTERVAL) {
//             timer_clear(GPT_TIM_1);
//             correspond_send_info_to_host();
//         }

        
        
//     }
// }



