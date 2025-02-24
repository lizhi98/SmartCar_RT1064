#ifndef _CODE_CORRESPOND_H_
#define _CODE_CORRESPOND_H_

#include "zf_driver_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_delay.h"
#include "motor.h"
#include "gyroscope.h"

#define WIFI_SSID   "SmartCar"
#define WIFI_PASS   "9894653xxk"

#define UART_N      UART_1
#define UART_BAUD   115200
#define UART_TX_PIN UART1_TX_B12
#define UART_RX_PIN UART1_RX_B13

typedef struct _RemoteControlItem{
    char * marker_word;

}RemoteControl;

uint8 wifi_spi_init_(uint16 time_out_s);

#endif