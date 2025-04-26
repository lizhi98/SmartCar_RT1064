// #include "zf_common_headfile.h"

// #include "main.h"



// vuint8 zebra_count = 0;
// // #define HOST_DEBUG
// // #define SEND_IMAGE
// // #define IPS_IMAGE

// // 斑马线有效标志
// uint8 zebra_valid_flag = 0;
// // 图像处理时间记录
// uint32 image_process_time_start = 0;
// uint32 image_process_time = 0;

// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M); //
//     // 陀螺仪初始化
//     gyroscope_init();
//     system_delay_ms(2000); // 延时等待陀螺仪稳定
//     gyroscope_pit_init();
//     system_delay_ms(2000); // 延时等待陀螺仪稳定
//                            //     // debug_init();                   // 调试端口初始化
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
//     mcx_uart_data_handle_pit_init();     // MCX_Vision 数据处理定时器初始化
//     // 摄像头初始化
//     hardware_init_flag += mt_camera_init();

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
//     if (hardware_init_flag)
//     {
//         screen_show_str(0, 0, "HW Init Fail!!!");
//         while (1)
//         {
//         };
//     } // 程序停止
//     else
//     {
//         screen_show_str(0, 0, "HW Init Success");
//     }

//     // system_delay_ms(1000);
//     screen_clear();
//     // =====================外设初始化结束========================

//     // 主循环准备
//     timer_init(GPT_TIM_1, TIMER_MS);
//     timer_start(GPT_TIM_1);
//     run_flag = 1;
//     char ips200_str_buffer[64];
//     char ips200_str_buffer_1[64];
//     char ips200_str_buffer_2[64];
//     char ips200_str_buffer_3[64];
//     char *image_ele_buffer;
//     // target_speed_magnitude = 700;
//     motion_control.motion_mode = LINE_FOLLOW; // 运动模式初始化
//     // uint32 zebra_time = 0;
//     // 主循环
//     float s_angle = gyroscope_result.angle_z;
//     while (1)
//     {
//         ips200_show_float(0, 0, gyroscope_result.angle_z, 4, 2); // 显示陀螺仪角度
//         if ((s_angle - gyroscope_result.angle_z + 90.0) < 0.)
//         {
//             int i = 1500;
//             while (i--)
//             {
//                 int32 motor_left_speed = 0,
//                       motor_right_speed = 0,
//                       motor_rear_speed = 0;
//                 // translation_pid.left_speed_out = 400;
//                 // translation_pid.front_speed_out = 400;
//                 // rotation_pid.wl_out = 0;
//                 motor_left_speed = (int32)(-400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
//                 motor_right_speed = (int32)(400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
//                 motor_rear_speed = (int32)(0);

//                 motor_left_speed += 0;
//                 motor_right_speed += 0;
//                 motor_rear_speed += 0;

//                 // 应用速度
//                 motor_run_with_speed(LEFT, motor_left_speed);
//                 motor_run_with_speed(RIGHT, motor_right_speed);
//                 motor_run_with_speed(REAR, motor_rear_speed);
//                 system_delay_ms(1);
//             }
//             motor_run_with_speed(LEFT, 0);
//             motor_run_with_speed(RIGHT, 0);
//             motor_run_with_speed(REAR, 0);
//             // while(1);
//             int j = 1500;
//             while (j--)
//             {
//                 int32 motor_left_speed = 0,
//                       motor_right_speed = 0,
//                       motor_rear_speed = 0;
//                 // translation_pid.left_speed_out = 400;
//                 // translation_pid.front_speed_out = 400;
//                 // rotation_pid.wl_out = 0;
//                 motor_left_speed = (int32)(400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
//                 motor_right_speed = (int32)(-400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
//                 motor_rear_speed = (int32)(0);

//                 motor_left_speed += 0;
//                 motor_right_speed += 0;
//                 motor_rear_speed += 0;

//                 // 应用速度
//                 motor_run_with_speed(LEFT, motor_left_speed);
//                 motor_run_with_speed(RIGHT, motor_right_speed);
//                 motor_run_with_speed(REAR, motor_rear_speed);
//                 system_delay_ms(1);
//             }
//             uint32 s_angle1 = gyroscope_result.angle_z;
//             while (1)
//             {
//                 if ((gyroscope_result.angle_z - s_angle1 + 90.0) < 0.)
//                 {
//                     break;
//                 }
//                 int32 motor_left_speed = 0,
//                       motor_right_speed = 0,
//                       motor_rear_speed = 0;
//                 // translation_pid.left_speed_out = 400;
//                 // translation_pid.front_speed_out = 400;
//                 // rotation_pid.wl_out = 0;
//                 motor_left_speed = (int32)(-0 * COS_PI_D_6 - 200 * SIN_PI_D_6);
//                 motor_right_speed = (int32)(0 * COS_PI_D_6 - 200 * SIN_PI_D_6);
//                 motor_rear_speed = (int32)(+200);

//                 motor_left_speed += 60;
//                 motor_right_speed += 60;
//                 motor_rear_speed += 60;

//                 // 应用速度
//                 motor_run_with_speed(LEFT, motor_left_speed);
//                 motor_run_with_speed(RIGHT, motor_right_speed);
//                 motor_run_with_speed(REAR, motor_rear_speed);
//             }

//             while (1)
//                 ;
//         }
//         int32 motor_left_speed = 0,
//               motor_right_speed = 0,
//               motor_rear_speed = 0;
//         // translation_pid.left_speed_out = 400;
//         // translation_pid.front_speed_out = 400;
//         // rotation_pid.wl_out = 0;
//         motor_left_speed = (int32)(-0 * COS_PI_D_6 + 200 * SIN_PI_D_6);
//         motor_right_speed = (int32)(0 * COS_PI_D_6 + 200 * SIN_PI_D_6);
//         motor_rear_speed = (int32)(-200);

//         motor_left_speed += -60;
//         motor_right_speed += -60;
//         motor_rear_speed += -60;

//         // 应用速度
//         motor_run_with_speed(LEFT, motor_left_speed);
//         motor_run_with_speed(RIGHT, motor_right_speed);
//         motor_run_with_speed(REAR, motor_rear_speed);
//     }
// }
