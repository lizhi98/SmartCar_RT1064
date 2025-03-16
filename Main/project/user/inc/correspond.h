#ifndef _CODE_CORRESPOND_H_
#define _CODE_CORRESPOND_H_

#include "zf_driver_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_delay.h"
#include "motor.h"
#include "gyroscope.h"

#define WIFI_SSID               "SmartCar"
#define WIFI_PASS               "9894653xxk"
#define WIFI_SPI_CONNECT_MODE   "TCP"

#define WIFI_SPI_RECV_PIT       PIT_CH2
#define WIFI_SPI_RECV_PIT_MS    500

#define host_to_rt_head '\r'
#define host_to_rt_tail '\n'

#define UART_N      UART_1
#define UART_BAUD   115200
#define UART_TX_PIN UART1_TX_B12
#define UART_RX_PIN UART1_RX_B13

// wifi_spi标志位
/*
*   0:空闲状态，扫描接收帧头
*   1:接收到帧头，扫描接收命令
*   2:接收到命令，扫描接收数据
*   3:接收完数据，准备接收帧尾
*/
typedef enum _wifi_spi_state{
    WIFI_SPI_AVAI     = 0,
    WIFI_SPI_RECVD_HEAD,
    WIFI_SPI_RECVD_CMD,
    WIFI_SPI_RECVD_DATA,
}wifi_spi_state;

extern wifi_spi_state wifi_spi_receive_flag;

// ================WIFI SPI====================
uint8 wifi_spi_init_(void);
uint8 wifi_spi_read(void * dst,uint32 dst_size);
void  wifi_spi_pit_call(void);
void  wifi_spi_pit_init(void);

#endif