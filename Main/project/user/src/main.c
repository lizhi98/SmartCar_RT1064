#include "zf_common_headfile.h"

#include "main.h"


// // 斑马线有效标志
uint8 zebra_valid_flag = 0;

uint32 image_process_time = 0; // 图像处理时间
uint32 image_process_wait_next_time = 0; // 图像等待处理时间

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);
    system_delay_ms(500);
    screen_init();         // 屏幕初始化
    menu_init();           // 菜单初始化
    mt_camera_init();       // 摄像头初始化
    // 陀螺仪初始化
    gyroscope_init();      // 陀螺仪初始化
    system_delay_ms(1000); // 等待陀螺仪初始化完成
    gyroscope_pit_init();  // 陀螺仪PIT定时器初始化
    system_delay_ms(2000); // 等待陀螺仪获取偏置数据
    // correspond_host_cmd_init();  // 上位机命令接收初始化
    encoder_all_init();       // 编码器初始化
    motor_encoder_pit_init(); // 编码器PIT定时器初始化
    motor_all_init();         // 电机初始化
    motion_control_pid_pit_init(); // PID定时器初始化
    // correspond_image_send_init(); // 图像发送初始化
    mcx_init_wait(); // MCX_Vision 唤醒
    mcx_receive_data_interrupt_enable(); // MCX_Vision 接收中断使能
    mcx_uart_data_handle_pit_init(); // MCX_Vision 数据处理 PIT定时器初始化
    art_init(); // OpenART 初始化
    art_receive_data_interrupt_enable(); // OpenART 接收中断使能
    // art_uart_data_handle_pit_init(); // OpenART 数据处理 PIT定时器初始化

    // 计时器初始化
    timer_init(GPT_TIM_1, TIMER_MS); // 定时器初始化
    timer_start(GPT_TIM_1); // 启动定时器

    change_page_to_debug_info(); // 切换到页面

    uint32 i = 0;
    motion_control.motion_mode = LINE_FOLLOW; // 设置初始运动模式为巡线
    // 初始化时间变量
    uint32 image_process_end_time = 0;
    uint32 image_process_start_time = 0;
    while (1)
    {
        if(motion_control.motion_mode == LINE_FOLLOW){
            if(mt9v03x_finish_flag){
                image_process_start_time = timer_get(GPT_TIM_1); // 获取图像处理开始时间
                image_process_wait_next_time = image_process_start_time - image_process_end_time;
                // process_image(mt9v03x_image); // 处理图像
                image_process_end_time = timer_get(GPT_TIM_1); // 获取图像处理结束时间
                image_process_time = image_process_end_time - image_process_start_time; // 计算图像处理时间
                // seekfree_assistant_camera_send(); // 发送图像到上位机
                image_show_flash(); // 刷新图像显示
                mt9v03x_finish_flag = 0; // 清除图像采集完成标志位
            }
        }
        debug_info_table_flash();
        motion_mode_calc();
        // ips200_show_int(0, 40, i++, 5); // 显示计数
        // correspond_send_info_to_host(); // 发送信息到上位机
    }    
}