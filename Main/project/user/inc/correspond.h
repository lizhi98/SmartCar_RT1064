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

#define UART_N      UART_1
#define UART_BAUD   115200
#define UART_TX_PIN UART1_TX_B12
#define UART_RX_PIN UART1_RX_B13

// ================WIFI SPI====================
uint8 wifi_spi_init_(void);
uint8 wifi_spi_read(void * dst,uint32 dst_size);


#endif