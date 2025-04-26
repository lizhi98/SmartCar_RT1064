#include "zf_common_headfile.h"

#include "main.h"

vuint8 zebra_count = 0;
// #define HOST_DEBUG 
// #define SEND_IMAGE
#define IPS_IMAGE

// 斑马线有效标志
uint8 zebra_valid_flag = 0;
// 图像处理时间记录
uint32 image_process_time_start = 0;
uint32 image_process_time = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
//     // debug_init();                   // 调试端口初始化
    // =====================外设初始化开始=========================
    // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
    uint8 hardware_init_flag = 0;
    gyroscope_init();
    system_delay_ms(2000); // 延时等待陀螺仪稳定
    gyroscope_pit_init();
    system_delay_ms(1000);
    // 屏幕初始化
    screen_init();

    // UART TO OpenART mini 初始化
    // TODO
    // UART TO MCX_Vision 初始化
    hardware_init_flag += mcx_init_wait();
    mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
    mcx_uart_data_handle_pit_init(); // MCX_Vision 数据处理定时器初始化
    // 摄像头初始化
    hardware_init_flag += mt_camera_init();
    // 陀螺仪初始化


    // 电机初始化
    motor_all_init();
    // 编码器初始化
    encoder_all_init();
    motor_encoder_pit_init();
    motion_pid_pit_init();    
#if defined(HOST_DEBUG)
    // 上位机控制初始化
    hardware_init_flag += correspond_host_cmd_init();
#endif
    // correspond_host_cmd_pit_init();
    // 图传初始化
#if defined(SEND_IMAGE)
    correspond_image_send_init();
#endif
    // 主循环计时器初始化
    timer_init(GPT_TIM_1, TIMER_MS);
    timer_start(GPT_TIM_1);

    // 初始化完成，屏幕输出初始化结果
    if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
    else                  {screen_show_str(0,0,"HW Init Success");}

    // system_delay_ms(1000);
    screen_clear();
    // =====================外设初始化结束========================

    // 主循环准备
    timer_init(GPT_TIM_1, TIMER_MS);
    timer_start(GPT_TIM_1);
    run_flag = 1;
    char    ips200_str_buffer[64];
    char    ips200_str_buffer_1[64];
    char    ips200_str_buffer_2[64];
    char    ips200_str_buffer_3[64];
    char *  image_ele_buffer;
    // target_speed_magnitude = 700;
    motion_control.motion_mode = LINE_FOLLOW; // 运动模式初始化
    // uint32 zebra_time = 0;
    // 主循环
    while(1) {


        // if(zebra_valid_flag == 0){
        //     if(timer_get(GPT_TIM_1) >= 10000){
        //         zebra_valid_flag = 1; // 斑马线有效
        //     }
        // }
        // 定时发送速度信息
//         if (timer_get(GPT_TIM_1) > WIFI_SPI_SEND_INTERVAL) {
// #if defined(HOST_DEBUG)
//             // correspond_send_info_to_host();
//             // timer_clear(GPT_TIM_1);
// #endif
//         }
        // 计算图像处理时间
        // 图像处理、发送与运动解算
        if (mt9v03x_finish_flag) {
            image_process_time_start = timer_get(GPT_TIM_1);
            
            process_image(mt9v03x_image);

#if defined(IPS_IMAGE)
            ips200_displayimage03x((uint8 *)&mt9v03x_image[0], MT9V03X_W, MT9V03X_H); // 显示图像

            switch(image_result.element_type){
                case Zebra: image_ele_buffer = "Zebra"; break;
                case CurveLeft: image_ele_buffer = "CurveL"; break;
                case CurveRight: image_ele_buffer = "CurveR"; break;
                case Normal: image_ele_buffer = "Normal"; break;
                case CrossBefore: image_ele_buffer = "CrossB"; break;
                case Cross: image_ele_buffer = "Cross"; break;
                case LoopLeftBefore: image_ele_buffer = "LoopLB"; break;
                case LoopRightBefore: image_ele_buffer = "LoopRB"; break;
                case LoopLeft: image_ele_buffer = "LoopL"; break;
                case LoopRight: image_ele_buffer = "LoopR"; break;
                case LoopLeftAfter: image_ele_buffer = "LoopLA"; break;
                case LoopRightAfter: image_ele_buffer = "LoopRA"; break;
                case RampLeft: image_ele_buffer = "RampL"; break;
                case RampRight: image_ele_buffer = "RampR"; break;
                default: image_ele_buffer = "Unknown"; break;
            }

            sprintf(ips200_str_buffer,"offset:%8.3lf,ele:%s,f:%1d",image_result.offset,image_ele_buffer,run_flag);
            ips200_show_string(0, MT9V03X_H, ips200_str_buffer); // 显示图像处理结果
            // sprintf(ips200_str_buffer_1,"state:%1d,x:%4d,y:%4d,mode:%1d",
            //     cube_info.state,cube_info.x_center,cube_info.y_center,motion_control.motion_mode);
            // sprintf(ips200_str_buffer_2,"yf:%5d,xf:%5d",
            //     translation_pid.front_offset,translation_pid.left_offset);
            // sprintf(ips200_str_buffer_3,"fs:%5d,ls:%5d,ro:%5d",
            //     translation_pid.front_speed_out,translation_pid.left_speed_out,
            //     rotation_pid.wl_out);
            // ips200_show_string(0, MT9V03X_H, ips200_str_buffer_1);
            // ips200_show_string(0, MT9V03X_H+30, ips200_str_buffer_2);
            // ips200_show_string(0, MT9V03X_H+60, ips200_str_buffer_3);
#endif
#if defined(SEND_IMAGE)
            // 发送图像到上位机
            seekfree_assistant_camera_send();
#endif
            image_process_time = timer_get(GPT_TIM_1) - image_process_time_start;          // 计算图像处理时间
            mt9v03x_finish_flag = 0;
        }
        if(image_result.element_type == Zebra){
            zebra_count++;
            if(zebra_count >=1){
                zebra_count = 0;
                run_flag = 0;
            }
        }
        // 运动解算        
        target_motion_calc();

    }
}

void push_box(){
    float s_angle = gyroscope_result.angle_z;
    while (1)
    {
        ips200_show_float(0, 0, gyroscope_result.angle_z, 4, 2); // 显示陀螺仪角度
        if ((s_angle - gyroscope_result.angle_z + 90.0) < 0.)
        {
            int i = 1800;
            while (i--)
            {
                int32 motor_left_speed = 0,
                      motor_right_speed = 0,
                      motor_rear_speed = 0;
                // translation_pid.left_speed_out = 400;
                // translation_pid.front_speed_out = 400;
                // rotation_pid.wl_out = 0;
                motor_left_speed = (int32)(-400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
                motor_right_speed = (int32)(400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
                motor_rear_speed = (int32)(0);

                motor_left_speed += 0;
                motor_right_speed += 0;
                motor_rear_speed += 0;

                // 应用速度
                motor_run_with_speed(LEFT, motor_left_speed);
                motor_run_with_speed(RIGHT, motor_right_speed);
                motor_run_with_speed(REAR, motor_rear_speed);
                system_delay_ms(1);
            }
            motor_run_with_speed(LEFT, 0);
            motor_run_with_speed(RIGHT, 0);
            motor_run_with_speed(REAR, 0);
            // while(1);
            int j = 1100;
            while (j--)
            {
                int32 motor_left_speed = 0,
                      motor_right_speed = 0,
                      motor_rear_speed = 0;
                // translation_pid.left_speed_out = 400;
                // translation_pid.front_speed_out = 400;
                // rotation_pid.wl_out = 0;
                motor_left_speed = (int32)(400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
                motor_right_speed = (int32)(-400 * COS_PI_D_6 + 0 * SIN_PI_D_6);
                motor_rear_speed = (int32)(0);

                motor_left_speed += 0;
                motor_right_speed += 0;
                motor_rear_speed += 0;

                // 应用速度
                motor_run_with_speed(LEFT, motor_left_speed);
                motor_run_with_speed(RIGHT, motor_right_speed);
                motor_run_with_speed(REAR, motor_rear_speed);
                system_delay_ms(1);
            }
            uint32 s_angle1 = gyroscope_result.angle_z;
            while (1)
            {
                if ((gyroscope_result.angle_z - s_angle1 + 90.0) < 0.)
                {
                    break;
                }
                int32 motor_left_speed = 0,
                      motor_right_speed = 0,
                      motor_rear_speed = 0;
                // translation_pid.left_speed_out = 400;
                // translation_pid.front_speed_out = 400;
                // rotation_pid.wl_out = 0;
                motor_left_speed = (int32)(-0 * COS_PI_D_6 - 200 * SIN_PI_D_6);
                motor_right_speed = (int32)(0 * COS_PI_D_6 - 200 * SIN_PI_D_6);
                motor_rear_speed = (int32)(+200);

                motor_left_speed += 60;
                motor_right_speed += 60;
                motor_rear_speed += 60;

                // 应用速度
                motor_run_with_speed(LEFT, motor_left_speed);
                motor_run_with_speed(RIGHT, motor_right_speed);
                motor_run_with_speed(REAR, motor_rear_speed);
            }

            return;
        }
        int32 motor_left_speed = 0,
              motor_right_speed = 0,
              motor_rear_speed = 0;
        // translation_pid.left_speed_out = 400;
        // translation_pid.front_speed_out = 400;
        // rotation_pid.wl_out = 0;
        motor_left_speed = (int32)(-0 * COS_PI_D_6 + 200 * SIN_PI_D_6);
        motor_right_speed = (int32)(0 * COS_PI_D_6 + 200 * SIN_PI_D_6);
        motor_rear_speed = (int32)(-200);

        motor_left_speed += -60;
        motor_right_speed += -60;
        motor_rear_speed += -60;

        // 应用速度
        motor_run_with_speed(LEFT, motor_left_speed);
        motor_run_with_speed(RIGHT, motor_right_speed);
        motor_run_with_speed(REAR, motor_rear_speed);
        // break;
    }
}
