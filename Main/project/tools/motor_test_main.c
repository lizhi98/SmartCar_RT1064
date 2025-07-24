// #include "zf_common_headfile.h"

// #include "main.h"

// // #define HOST_DEBUG 
// // #define SEND_IMAGE
// // #define IPS_IMAGE
// // #define CUBE_INFO_SHOW

// // 斑马线有效标志
// uint8 zebra_valid_flag = 0;
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
//     gyroscope_init();
//     system_delay_ms(2000); // 延时等待陀螺仪稳定
//     gyroscope_pit_init();
//     system_delay_ms(1000); // 延时等待陀螺仪获取平均噪声
//     // 屏幕初始化
//     screen_init();

//     // UART TO OpenART mini 初始化
//     // TODO
//     // UART TO MCX_Vision 初始化
//     hardware_init_flag += mcx_init_wait();
//     mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
//     mcx_uart_data_handle_pit_init(); // MCX_Vision 数据处理定时器初始化
//     // 摄像头初始化
//     hardware_init_flag += mt_camera_init();
//     // 陀螺仪初始化
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

//     // motion_control.motion_mode = LINE_FOLLOW; // 运动模式初始化

//     // 主循环
//     run_flag = 1;
//     uint16 image_send_count = 0;
//     uint16 image_time_show_ips = 0;
//     while(1) {

//         if (mt9v03x_finish_flag)
//         {
//             image_process_time_start = timer_get(GPT_TIM_1);
            
//             process_image(mt9v03x_image);
            
//             // 累加offset
//             line_offset.sum_offset += image_result.offset;
//             line_offset.times++;
            
//             image_process_time = timer_get(GPT_TIM_1) - image_process_time_start;          // 计算图像处理时间

//             if(1){
//                 // ips200_show_uint(0, 0, ++image_time_show_ips,5);
// #if defined(SEND_IMAGE)
//                 seekfree_assistant_camera_send();
// #endif
//                 image_send_count = 0;
//             }
//             if(zebra_valid_flag && (image_result.element_type == Zebra)){
//                 run_flag = 0;
//             }
            
// #if defined(IPS_IMAGE)
//             screen_show_info();
// #endif
//             mt9v03x_finish_flag = 0;
//         }
// #if defined(HOST_DEBUG)
//         correspond_send_info_to_host();
// #endif
//         if(timer_get(GPT_TIM_1) > 3000){
//             zebra_valid_flag = 1;
//         }
        
//         // 运动解算        
//         target_motion_calc();

//     }
// }

// void push_box(){
//     // 初始化三个电机速度缓存
//     int32   motor_left_speed    = 0,
//             motor_right_speed   = 0,
//             motor_rear_speed    = 0;
            
//     float s_angle = gyroscope_result.angle_z;
//     motion_control.motion_mode = CUBE_ANGLE_POSITION_LEFT;
//     // 转90度
//     while(1){
//         if((gyroscope_result.angle_z - s_angle ) >  90.0){
//             motor_run_with_speed(LEFT,0);
//             motor_run_with_speed(RIGHT,0);
//             motor_run_with_speed(REAR,0);
//             break;
//         }

        
//         motor_left_speed    =   (int32) (170* SIN_PI_D_6);
//         motor_right_speed   =   (int32) (170 * SIN_PI_D_6);
//         motor_rear_speed    =   (int32) (-170);

//         motor_left_speed    +=  -50;
//         motor_right_speed   +=  -50;
//         motor_rear_speed    +=  -50;
        
//         // 应用速度
//         motor_run_with_speed(LEFT,motor_left_speed);
//         motor_run_with_speed(RIGHT,motor_right_speed);
//         motor_run_with_speed(REAR,motor_rear_speed);
//     }

//     system_delay_ms(500); // 转90度时间
    
//     // while(!cube_distance_position_ok()){
//     //     motion_control.motion_mode = CUBE_DISTANCE_POSITION;
//     //     if(cube_info.state == CUBE_OUTSIDE_VIEW){
//     //         goto angle_back;
//     //         motion_control.motion_mode = LINE_FOLLOW;
//     //         return;
//     //     }
//     // }
//     motion_control.motion_mode = CUBE_PUSH;
//     system_delay_ms(500);
// cube_push:
//     // 推箱子
//     motion_control.motion_mode = CUBE_PUSH;
//     // 初始化三个电机速度缓存
//     motor_left_speed    = 0,
//     motor_right_speed   = 0,
//     motor_rear_speed    = 0;
    
//     motor_left_speed    =   (int32) (-150 * COS_PI_D_6);
//     motor_right_speed   =   (int32) ( 150 * COS_PI_D_6);
//     motor_rear_speed    =   (int32) (0.);
    
//     // 应用速度
//     motor_run_with_speed(LEFT,motor_left_speed);
//     motor_run_with_speed(RIGHT,motor_right_speed);
//     motor_run_with_speed(REAR,motor_rear_speed);

//     system_delay_ms(2300); // 推箱子时间
// line_back:
//     motion_control.motion_mode = CUBE_ANGLE_POSITION_LEFT;
//     // 返回
//     motor_left_speed    = 0,
//     motor_right_speed   = 0,
//     motor_rear_speed    = 0;

//     motor_left_speed    =   (int32) (150 * COS_PI_D_6);
//     motor_right_speed   =   (int32) (-150 * COS_PI_D_6);
//     motor_rear_speed    =   (int32) (0.);
    
//     // 应用速度
//     motor_run_with_speed(LEFT,motor_left_speed);
//     motor_run_with_speed(RIGHT,motor_right_speed);
//     motor_run_with_speed(REAR,motor_rear_speed);

//     system_delay_ms(2300); // 返回时间


//     s_angle = gyroscope_result.angle_z;
//     // 转回到90度
// angle_back:
//     motion_control.motion_mode = LINE_BACK;
//     while(1){
//         if((s_angle - gyroscope_result.angle_z) > 90.0){
//             motor_run_with_speed(LEFT,0);
//             motor_run_with_speed(RIGHT,0);
//             motor_run_with_speed(REAR,0);
//             break;
//         }
//         // 初始化三个电机速度缓存
//         motor_left_speed    = 0,
//         motor_right_speed   = 0,
//         motor_rear_speed    = 0;
        
//         motor_left_speed    =   (int32) (-170* SIN_PI_D_6);
//         motor_right_speed   =   (int32) (-170 * SIN_PI_D_6);
//         motor_rear_speed    =   (int32) (170);

//         motor_left_speed    +=  50;
//         motor_right_speed   +=  50;
//         motor_rear_speed    +=  50;
        
//         // 应用速度
//         motor_run_with_speed(LEFT,motor_left_speed);
//         motor_run_with_speed(RIGHT,motor_right_speed);
//         motor_run_with_speed(REAR,motor_rear_speed);
//     }

//     system_delay_ms(1000); // 转90度时间
//     motion_control.motion_mode = LINE_FOLLOW;
// }

// #include "zf_common_headfile.h"

// #include "main.h"

// // 斑马线有效标志
// uint8 zebra_valid_flag = 0;

// uint32 image_process_time = 0; // 图像处理时间
// uint32 image_process_wait_next_time = 0; // 图像等待处理时间

// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M);
//     system_delay_ms(1000);
//     pwm_init(PWM2_MODULE2_CHB_C11, 17000, 2000);         // PWM 初始化
//     gpio_init(C10, GPO, 0, GPO_PUSH_PULL); // GPIO 初始化
//     // system_delay_ms(5000); // 延时等待 PWM 初始化完成
//     // pwm_set_duty(    PWM2_MODULE0_CHB_C7, 0); // 设置 PWM 占空比
// 		// 左 0 顺时针
// 		// 右 0 顺时针
// 		// 后 0 顺时针
//     while(1) {
//         // system_delay_ms(5000); 
//         // gpio_toggle_level(C6); // 切换 GPIO 电平
//     }
// }