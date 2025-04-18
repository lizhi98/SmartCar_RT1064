#include "MCX_Vision.h"
#include "zf_common_fifo.h"
#include "zf_driver_timer.h"
#include "zf_device_ips200.h"

uint8 mcx_init_flag = 0; // MCX模块初始化标志位

CubeLocationOffset cube_location_offset = {0, 0};

// MCX_Vision 缓冲区fifo
fifo_struct mcx_uart_fifo;
uint8 mcx_uart_buffer[MCX_READ_BUFFER_LEN];

uint8 mcx_uart_interrupt_data_temp;

/*
* 函数简介     MCX模块UART通信初始化函数
* 参数说明     void
* 返回参数     0:成功 1:失败
* 使用示例     mcx_init_wait();
* 备注信息     初始化MCX模块，等待MCX模块准备好，超时返回失败
*/
uint8 mcx_init_wait(void){
    // 初始化UART
    uart_init(MCX_UART_N,MCX_UART_BAUD,MCX_UART_TX,MCX_UART_RX);

    timer_init(GPT_TIM_1, TIMER_MS); // 初始化定时器
    timer_start(GPT_TIM_1); // 启动定时器
    uint32 time_out = timer_get(GPT_TIM_1); // 获取定时器计数值
    uint8 dat_temp = 0x00;
    // 等待MCX模块准备好
    while(1){
        
        time_out = timer_get(GPT_TIM_1) - time_out; // 获取定时器计数值
        if(time_out > MCX_UART_INIT_TIME_OUT_MS){ // 超时退出
            timer_stop(GPT_TIM_1); // 停止定时器
            return 1u; // 超时失败
        }

        // 每隔10ms发送0x01数据
        uart_write_byte(MCX_UART_N,MCX_UART_INIT_WAKE_DATA);
        system_delay_ms(10); // 延时10ms

        uart_query_byte(MCX_UART_N, &dat_temp);

        if(dat_temp == MCX_UART_INIT_WAKE_DATA){
            if(fifo_init(&mcx_uart_fifo, FIFO_DATA_8BIT, mcx_uart_buffer, MCX_READ_BUFFER_LEN) == FIFO_SUCCESS){
                mcx_init_flag = 1u; // MCX模块初始化成功
                return 0u;
            }else{
                timer_stop(GPT_TIM_1); // 停止定时器
                return 1u; // FIFO初始化失败
            }
        }   
    }

}

void mcx_receive_data_call(void){
    // 未初始化则return
    if(!mcx_init_flag){
        return;
    }
    uart_query_byte(MCX_UART_N, &mcx_uart_interrupt_data_temp);                    // 读取串口数据
    fifo_write_buffer(&mcx_uart_fifo, &mcx_uart_interrupt_data_temp, 1);           // 存入 FIFO
}
void mcx_test(void){

}
