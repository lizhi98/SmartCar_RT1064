#include "MCX_Vision.h"
#include "zf_common_headfile.h"
#include "main.h"

uint32 mcx_data_length = MCX_DATA_LENGTH - 1;

vuint8 mcx_init_flag = 0; // MCX模块初始化标志位

vuint8 * scc8660_recv_p;

CubeInfo cube_info = {.state = CUBE_OUTSIDE_VIEW, .x_offset = 0, .y_offset = 0}; // 立方体信息

// MCX_Vision 数据缓冲和标志位
fifo_struct mcx_uart_fifo;
uint8 mcx_uart_fifo_buffer[MCX_READ_BUFFER_LEN];
uint8 mcx_data_buffer[MCX_DATA_LENGTH];

McxUartState mcx_uart_state = { .is_receiving = false, .is_done = false, .offset = 0 };

/*
* 函数简介     MCX模块UART通信初始化函数
* 参数说明     void
* 返回参数     0:成功 1:失败
* 使用示例     mcx_init_wait();
* 备注信息     初始化MCX模块，等待MCX模块准备好，超时返回失败
*/
uint8 mcx_init_wait(void) {
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
            if(fifo_init(&mcx_uart_fifo, FIFO_DATA_8BIT, mcx_uart_fifo_buffer, MCX_READ_BUFFER_LEN) == FIFO_SUCCESS){
                mcx_init_flag = 1u; // MCX模块初始化成功
                return 0u;
            }else{
                timer_stop(GPT_TIM_1); // 停止定时器
                return 1u; // FIFO初始化失败
            }
        }   
    }

}

void mcx_receive_data_callback(void) {
    if (! mcx_init_flag || mcx_uart_state.is_done) return;

    uint8 b;
    uart_query_byte(MCX_UART_N, &b); // 从串口读一字节

    if (mcx_uart_state.is_receiving) {
        if (mcx_uart_state.offset == SCC8660_IMAGE_SIZE) {
            mcx_uart_state.is_receiving = false;
            mcx_uart_state.offset = 0;
            mcx_uart_state.is_error = b != MCX_BUFFER_TAIL;
        }
    }
    else {
        vuint8* p = (vuint8*) &scc8660_image_buffer[0];
        p[mcx_uart_state.offset ++] = b;
    }
    mcx_uart_state.is_done = true;
}

void mcx_receive_data_interrupt_enable(){
    uart_rx_interrupt(MCX_UART_N, 1); // 打开接收中断
}

void mcx_receive_data_interrupt_disable(){
    uart_rx_interrupt(MCX_UART_N, 0); // 打开接收中断
}

void mcx_cube_data_handle_pit_call(){
    // // 未初始化或数据接收未完成则return
    // if(!mcx_init_flag || !mcx_uart_state){
    //     return;
    // }
    // // 先查询现在有多少数据
    // if(fifo_used(&mcx_uart_fifo) != MCX_DATA_LENGTH - 1){ // 数据长度异常
    //     mcx_uart_state = 0; // 打回，重新接收
    //     return;
    // }else{
    //     // 程序执行到这应该是正常数据了
    //     fifo_read_buffer(&mcx_uart_fifo, mcx_data_buffer, &mcx_data_length, FIFO_READ_AND_CLEAN); // 读取数据
    //     if(mcx_data_length == MCX_DATA_LENGTH - 1){ // 数据长度正常
    //         // 加上\0
    //         mcx_data_buffer[MCX_DATA_LENGTH - 1] = '\0'; // 添加结束符
    //         // 解析数据
    //         sscanf((char *)&mcx_data_buffer[0],"s:%d,xf:%d,yf:%d\n",&cube_info.state,&cube_info.x_offset,&cube_info.y_offset); // 解析数据
    //     }else{
    //         mcx_data_length = MCX_DATA_LENGTH - 1; // 重置数据长度,防止fifo_read_buffer修改length
    //     }
    //     mcx_uart_state = 0;
    // }
}
