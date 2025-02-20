#ifndef _CODE_CORRESPOND_H_
#define _CODE_CORRESPOND_H_

#include "zf_driver_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_delay.h"

#define WIFI_SSID   "AP_SmartCar"
#define WIFI_PASS   "SMARTCAR"

#define UART_N      UART_1
#define UART_BAUD   115200
#define UART_TX_PIN UART1_TX_B12
#define UART_RX_PIN UART1_RX_B13

uint8 * TO_HOST_CMD_HEAD = "THCS";
uint8 * TO_HOST_CMD_TAIL = "THCE";
uint8 * TO_ART_CMD_HEAD = "TOCS";
uint8 * TO_ART_CMD_TAIL = "TOCE";

uint8 * TO_HOST_DATA_HEAD = "THDS";
uint8 * TO_HOST_DATA_TAIL = "THDE";
uint8 * TO_ART_DATA_HEAD = "TODS";
uint8 * TO_ART_DATA_TAIL = "TODE";

uint8 wifi_spi_init_(uint16 time_out_s);

#endif