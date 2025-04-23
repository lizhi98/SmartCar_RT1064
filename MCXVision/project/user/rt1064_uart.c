#include "rt1064_uart.h"

uint8  rt1064_uart_read_buffer[RT1064_UART_READ_BUFFER_SIZE]; // 接收缓冲区
vuint8 rt1064_uart_read_buffer_index = 0; // 接收缓冲区索引
vuint8 rt1064_uart_read_buffer_finish = 0; // 接收到帧尾标志

uint8  rt1064_uart_write_buffer[RT1064_UART_READ_BUFFER_SIZE]; // 发送缓冲区

void rt1064_uart_receive_callback(uint8 data){
    if(rt1064_uart_read_buffer_finish){
        return;
    }

    if(data == RT1064_BUFFER_HEAD){
        rt1064_uart_read_buffer_index = 0; // 重置索引
    } else if(data == RT1064_BUFFER_TAIL){
        // 处理接收到的数据
        rt1064_uart_read_buffer_finish = 1; // 设置接收完成标志
    } else {
        if(rt1064_uart_read_buffer_index < RT1064_UART_READ_BUFFER_SIZE){
            rt1064_uart_read_buffer[rt1064_uart_read_buffer_index] = data; // 存储数据
            rt1064_uart_read_buffer_index++; // 增加索引
        }else{
            // 缓冲区溢出处理
            rt1064_uart_read_buffer_index = 0;
        }
    }
}


void rt1064_uart_init_wait(){
    user_uart_init();
    rt1064_uart_read_buffer_index = 0; // 初始化索引
    rt1064_uart_read_buffer_finish = 0; // 初始化接收完成标志

    while(1){
        if(rt1064_uart_read_buffer_index != 0){ // 如果没有接收到数据
            if(rt1064_uart_read_buffer[rt1064_uart_read_buffer_index - 1] == RT1064_UART_INIT_WAKE_DATA){
                user_uart_putchar(RT1064_UART_INIT_WAKE_DATA); // 发送唤醒数据
                return;
            }
        }
    }
}

void rt1064_uart_send_cube_info(void){
    sprintf((char *)&rt1064_uart_write_buffer[0],"s:%1d,xf:%4d,yf:%4d\n",cube_info.state,cube_info.x_offset,cube_info.y_offset);
    rt1064_uart_write_buffer[RT1064_CUBE_DATA_LENGTH - 1] = 0; // 确保\0存在
    user_uart_putchar(RT1064_BUFFER_HEAD); // 发送帧头
    user_uart_write_string((char *)&rt1064_uart_write_buffer[0]); // 发送数据
    user_uart_putchar(RT1064_BUFFER_TAIL); // 发送帧尾
}
