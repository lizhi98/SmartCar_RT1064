#ifndef _CODE_CORRESPOND_H_
#define _CODE_CORRESPOND_H_

#include "string.h"
#include "math.h"
#include "stdio.h"

#include "zf_driver_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_device_wifi_uart.h"
#include "seekfree_assistant_interface.h"
#include "seekfree_assistant.h"

#include "motor.h"
#include "gyroscope.h"

// WIRELESS CONFIG
#define WIFI_SSID                               "AP-lizhi"
#define WIFI_PASS                               "9894653xxk"
#define HOST_IP                                 "192.168.201.252"

// WIFI SPI CONFIG
#define WIFI_SPI_CONNECT_MODE                   "UDP"
#define WIFI_SPI_HOST_PORT                      "9894"

// WIFI UART CONFIG
#define WIFI_UART_HOST_PORT                     "9895"
#define WIFI_UART_READ_BUFFER_TEMP_SIZE         24
#define WIFI_UART_READ_BUFFER_HEAD              "[["
#define WIFI_UART_READ_BUFFER_HEAD_SIZE         2

#define WIFI_UART_SEND_BUFFER_TEMP_SIZE         64
#define CORRESPOND_SEND_INFO_MODE               2 // 1-发送电机速度，offset，PWM 2-发送陀螺仪数据

// UART CONFIG
#define UART_N      UART_4
#define UART_BAUD   115200
// #define UART_TX_PIN UART1_TX_B12
// #define UART_RX_PIN UART1_RX_B13

// CORRESPOND HOST CMD CONFIG
#define HOST_CMD_RECV_PIT          PIT_CH2
#define HOST_CMD_RECV_PIT_MS       100
#define HOST_CMD_SIZE_PLUS_ONE     12 // plus one是为了加上\0


extern uint8 wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_TEMP_SIZE];
extern char  host_cmd_recv_temp[HOST_CMD_SIZE_PLUS_ONE];

// ================WIFI_SPI==========================================
uint8 wifi_spi_read(void * dst,uint32 dst_size_n_uint8);

// ================WIFI_UART=========================================
uint8 wifi_uart_read_n_data(void * dst,uint32 dst_size_n_uint8);

// uint8 wifi_uart_read_int32  (int32 * dst,   int32  data_lenth_n);
// uint8 wifi_uart_read_float  (float * dst,   int32  data_lenth_n);
// uint8 wifi_uart_read_double (double * dst,  int32  data_lenth_n);

// ================CORRESPOND_HOST_CMD(WIFI_UART)====================
uint8 correspond_host_cmd_init(void);
void  correspond_host_cmd_pit_call(void);
void  correspond_host_cmd_pit_init(void);
void  correspond_send_info_to_host(void);

// ================CORRESPOND_IMAGE_SEND(WIFI_SPI)====================

uint8 correspond_image_send_init(void);
void  correspond_image_send_call(void);

#endif