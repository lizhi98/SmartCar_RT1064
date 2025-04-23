// #include "main.h"

// vuint16 scc8660_image_buffer[SCC8660_H * SCC8660_W];

// // 图像处理时间记录
// uint32 image_process_time_start = 0;
// uint32 image_process_time = 0;

// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M);  // 不可删除
// //     // debug_init();                   // 调试端口初始化
//     // =====================外设初始化开始=========================
//     // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
//     uint8 hardware_init_flag = 0;
//     // 屏幕初始化
//     screen_init();

//     // UART TO OpenART mini 初始化
//     // TODO
//     // UART TO MCX_Vision 初始化
//     hardware_init_flag += mcx_init_wait();
//     mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
//     // pit_ms_init(PIT_CH3, 20); // MCX_Vision 定时器初始化
//     // 图传初始化

//     correspond_image_send_init();
//     // system_delay_ms(1000);
//     screen_clear();
//     // =====================外设初始化结束========================

//     // 主循环准备
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);

//     // 主循环
//     while (1) {
//         if (mcx_uart_state.is_done) {
//             // seekfree_assistant_camera_send();
//             ips200_displayimage8660(scc8660_image_buffer, SCC8660_W, SCC8660_H); // 显示图像
//             mcx_uart_state.is_done = false;
//         }
//     }
// }

