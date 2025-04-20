#ifndef _CODE_MCX_VISION_H_
#define _CODE_MCX_VISION_H_

#include "stdint.h"

#include "zf_driver_uart.h"
#include "zf_driver_delay.h"
#include "zf_common_fifo.h"
#include "zf_driver_timer.h"
#include "zf_device_ips200.h"

#define MCX_UART_N                      UART_4
#define MCX_UART_BAUD                   115200
#define MCX_UART_TX                     UART4_TX_C16
#define MCX_UART_RX                     UART4_RX_C17

#define MCX_UART_INIT_TIME_OUT_MS       2000       
#define MCX_UART_INIT_WAKE_DATA         0x01

#define MCX_READ_BUFFER_LEN             64
#define MCX_BUFFER_HEAD                 0xFE
#define MCX_BUFFER_TAIL                 0xEF
#define MCX_DATA_LENGTH                 21

typedef enum _CubeDetectionState{
    CUBE_INSIDE_VIEW,
    CUBE_OUTSIDE_VIEW,
}CubeDetectionState;

typedef struct _CubeInfo{
    CubeDetectionState state;
    int32 x_offset;
    int32 y_offset;
}CubeInfo;

extern CubeInfo cube_info;

uint8 mcx_init_wait(void);
void  mcx_receive_data_callback(void);

void mcx_receive_data_interrupt_enable();
void mcx_receive_data_interrupt_disable();
void mcx_cube_data_handle_pit_call();

#endif