// #include "zf_common_headfile.h"

// #include "main.h"

// #define HOST_DEBUG
// // #define SEND_IMAGE
// #define IPS_IMAGE

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
//     // mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
//     // pit_ms_init(PIT_CH3, 20); // MCX_Vision 定时器初始化
//     // 摄像头初始化
//     hardware_init_flag += mt_camera_init();
//     // 陀螺仪初始化
//     gyroscope_init();
//     system_delay_ms(2000); // 延时等待陀螺仪稳定
//     gyroscope_pit_init();

//     // 电机初始化
//     motor_all_init();
//     // 编码器初始化
//     encoder_all_init();
//     motor_encoder_pit_init();
//     motion_pid_pit_init();    
// #if defined(HOST_DEBUG)
//     // 上位机控制初始化
//     hardware_init_flag += correspond_host_cmd_init();
// #endif
//     // correspond_host_cmd_pit_init();
//     // 图传初始化
// #if defined(SEND_IMAGE)
//     correspond_image_send_init();
// #endif
//     // 主循环计时器初始化
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);

//     // 初始化完成，屏幕输出初始化结果
//     if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
//     else                  {screen_show_str(0,0,"HW Init Success");}

//     // system_delay_ms(1000);
//     screen_clear();
//     // =====================外设初始化结束========================

//     // 主循环准备
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);

//     char    ips200_str_buffer[64];
//     char *  image_ele_buffer;
//     // target_speed_magnitude = 700;
//     motion_control.motion_mode = LINE_FOLLOW; // 运动模式初始化
//     // 主循环
//     while(1) {
//         // 定时发送速度信息
//         if (timer_get(GPT_TIM_1) > WIFI_SPI_SEND_INTERVAL) {
// #if defined(HOST_DEBUG)
//             correspond_send_info_to_host();
//             timer_clear(GPT_TIM_1);
// #endif
//         }

//         // 计算图像处理时间
//         // 图像处理、发送与运动解算
//         if (mt9v03x_finish_flag) {
//             image_process_time_start = timer_get(GPT_TIM_1);
            
//             process_image(mt9v03x_image);
// #if defined(IPS_IMAGE)
//             ips200_displayimage03x((uint8 *)&mt9v03x_image[0], MT9V03X_W, MT9V03X_H); // 显示图像

//             switch(image_result.element_type){
//                 case Zebra: image_ele_buffer = "Zebra"; break;
//                 case CurveLeft: image_ele_buffer = "CurveL"; break;
//                 case CurveRight: image_ele_buffer = "CurveR"; break;
//                 case Normal: image_ele_buffer = "Normal"; break;
//                 case CrossBefore: image_ele_buffer = "CrossB"; break;
//                 case Cross: image_ele_buffer = "Cross"; break;
//                 case LoopLeftBefore: image_ele_buffer = "LoopLB"; break;
//                 case LoopRightBefore: image_ele_buffer = "LoopRB"; break;
//                 case LoopLeft: image_ele_buffer = "LoopL"; break;
//                 case LoopRight: image_ele_buffer = "LoopR"; break;
//                 case LoopLeftAfter: image_ele_buffer = "LoopLA"; break;
//                 case LoopRightAfter: image_ele_buffer = "LoopRA"; break;
//                 case RampLeft: image_ele_buffer = "RampL"; break;
//                 case RampRight: image_ele_buffer = "RampR"; break;
//                 default: image_ele_buffer = "Unknown"; break;
//             }

//             sprintf(ips200_str_buffer,"offset:%8.3lf,ele:%s",image_result.offset,image_ele_buffer);
//             ips200_show_string(0, MT9V03X_H, ips200_str_buffer); // 显示图像名称
// #endif
// #if defined(SEND_IMAGE)
//             // 发送图像到上位机
//             seekfree_assistant_camera_send();
// #endif
//             image_process_time = timer_get(GPT_TIM_1) - image_process_time_start;          // 计算图像处理时间
//             mt9v03x_finish_flag = 0;
//         }

//         // 运动解算        
//         target_motion_calc();

//     }
// }

