#include "MCX_Vision.h"
#include "zf_common_headfile.h"
#include "main.h"

uint32 mcx_data_length = MCX_DATA_LENGTH - 1;

vuint8 mcx_init_flag = 0; // MCX模块初始化标志位
vuint8 mcx_data_recv_finish_flag = 0; // MCX模块数据接收完成标志位

CubeInfo cube_info ={
    .state = CUBE_OUTSIDE_VIEW,
    .x_center = 0,
    .y_center = 0,
};

// MCX_Vision 数据缓冲和标志位
fifo_struct mcx_uart_fifo;
uint8 mcx_uart_fifo_buffer[MCX_READ_BUFFER_LEN];
uint8 mcx_data_buffer[MCX_DATA_LENGTH];
vuint8 mcx_uart_interrupt_data_temp = 0; // 中断接收数据缓存

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
    if (! mcx_init_flag || mcx_data_recv_finish_flag) return;
    // 现在可以接收数据了
    uart_query_byte(MCX_UART_N, (uint8 *)&mcx_uart_interrupt_data_temp);                    // 读取串口数据

    if(mcx_uart_interrupt_data_temp == MCX_BUFFER_HEAD){ // 判断数据头
        fifo_clear(&mcx_uart_fifo); // 清空FIFO
        // mcx_data_recv_finish_flag = 0; // 数据接收开始
    }else if(mcx_uart_interrupt_data_temp == MCX_BUFFER_TAIL){ // 判断数据尾
        if(fifo_used(&mcx_uart_fifo) != MCX_DATA_LENGTH - 1){ // 数据长度异常
            mcx_data_recv_finish_flag = 0; // 打回，重新接收
            return;
        }else{
            mcx_data_recv_finish_flag = 1; // 数据接收完成
        }
    }else{
        fifo_write_buffer(&mcx_uart_fifo, (uint8*)&mcx_uart_interrupt_data_temp, 1);           // 存入 FIFO
    }
}

void mcx_receive_data_interrupt_enable(){
    uart_rx_interrupt(MCX_UART_N, 1); // 打开接收中断
}

void mcx_receive_data_interrupt_disable(){
    uart_rx_interrupt(MCX_UART_N, 0); // 关闭接收中断
}

void mcx_cube_data_handle_pit_call(){
    // 未初始化或数据接收未完成则return
    if(!mcx_init_flag || !(mcx_data_recv_finish_flag)){
        return;
    }
    // 先查询现在有多少数据
    if(fifo_used(&mcx_uart_fifo) != MCX_DATA_LENGTH - 1){ // 数据长度异常
        mcx_data_recv_finish_flag = 0; // 打回，重新接收
        return;
    }else{
        // 程序执行到这应该是正常数据了
        fifo_read_buffer(&mcx_uart_fifo, mcx_data_buffer, &mcx_data_length, FIFO_READ_AND_CLEAN); // 读取数据
        if(mcx_data_length == MCX_DATA_LENGTH - 1){ // 数据长度正常
            // 加上\0
            mcx_data_buffer[MCX_DATA_LENGTH - 1] = '\0'; // 添加结束符
            // 解析数据
            sscanf((char *)&mcx_data_buffer[0],"s:%d,xf:%d,yf:%d\n",&cube_info.state,&cube_info.x_center,&cube_info.y_center); // 解析数据
        }else{
            mcx_data_length = MCX_DATA_LENGTH - 1; // 重置数据长度,防止fifo_read_buffer修改length
        }
        mcx_data_recv_finish_flag = 0;
    }
}

void mcx_uart_data_handle_pit_init(){
    pit_ms_init(PIT_CH3, 20); // MCX_Vision 定时器初始化
}
