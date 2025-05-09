// #include "zf_common_headfile.h"

// #include "main.h"


// // // 斑马线有效标志
// uint8 zebra_valid_flag = 0;
// // // 图像处理时间记录
// uint32 image_process_time_start = 0;
// uint32 image_process_time = 0;


// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M); //
//     screen_init(); // 屏幕初始化
//     screen_clear();
//     correspond_host_cmd_init(); // 上位机控制初始化
//     // =====================外设初始化结束========================
//     uint8 buffer[64];
//     encoder_dir_init(motors[REAR].encoder->encoder_index, motors[REAR].encoder->encoder_channel_1, motors[REAR].encoder->encoder_channel_2);
//     while (1)
//     {
//         system_delay_ms(200);
//         sprintf((char *)buffer, "rear:%d", encoder_get_count(motors[REAR].encoder->encoder_index));
//         wifi_uart_send_buffer(buffer, strlen((char *)buffer));
//     }    
// }
