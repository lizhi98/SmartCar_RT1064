#ifndef _USER_RT1064_H_
#define _USER_RT1064_H_

#include "zf_driver_uart.h"
#include "cube_detection.h"

#define RT1064_UART_READ_BUFFER_SIZE        64 // 接收缓冲区大小
#define RT1064_BUFFER_HEAD                  0xFE
#define RT1064_BUFFER_TAIL                  0xEF
#define RT1064_UART_INIT_WAKE_DATA          0x01

extern uint8 rt1064_uart_read_buffer[RT1064_UART_READ_BUFFER_SIZE]; // 接收缓冲区
extern vuint8 rt1064_uart_read_buffer_index;// 接收缓冲区索引
extern vuint8 rt1064_uart_read_buffer_finish; // 接收到帧尾标志

void rt1064_uart_receive_callback(uint8 data);
void rt1064_uart_init_wait(void);

void rt1064_uart_send_cube_location(void);

#endif