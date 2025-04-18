#ifndef _CODE_MCX_VISION_H_
#define _CODE_MCX_VISION_H_

#include "zf_driver_uart.h"
#include "zf_driver_delay.h"

#define MCX_UART_N                      UART_4
#define MCX_UART_BAUD                   115200
#define MCX_UART_TX                     UART4_TX_C16
#define MCX_UART_RX                     UART4_RX_C17

#define MCX_UART_INIT_TIME_OUT_MS       2000       
#define MCX_UART_INIT_WAKE_DATA         0x01

#define MCX_READ_BUFFER_LEN             64
#define MCX_BUFFER_HEAD                 0xFE
#define MCX_BUFFER_TAIL                 0xEF
#define MCX_DATA_SIZE                   8

    typedef struct _CubeLocationOffset{
        int32 x_offset;
        int32 y_offset;
    }CubeLocationOffset;

extern CubeLocationOffset cube_location_offset;

uint8 mcx_init_wait(void);
void  mcx_receive_data_call(void);
void  mcx_test(void);

#endif