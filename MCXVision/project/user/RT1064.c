#include "RT1064.h"

uint8  rt1064_uart_read_buffer[RT1064_UART_READ_BUFFER_SIZE]; // 接收缓冲区
vuint8 rt1064_uart_read_buffer_index = 0; // 接收缓冲区索引
vuint8 rt1064_uart_read_buffer_finish = 0; // 接收到帧尾标志

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
                // while(1);
                return;
            }
        }
    }
}

void rt1064_uart_send_cube_location(void){
    
}
