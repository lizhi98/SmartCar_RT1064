#ifndef _CODE_MCX_VISION_H_
#define _CODE_MCX_VISION_H_

#include "stdint.h"

#include "zf_driver_uart.h"
#include "zf_driver_delay.h"
#include "zf_common_fifo.h"
#include "zf_driver_timer.h"
#include "zf_device_ips200.h"

#define MCX_UART_N                      UART_4
#define MCX_UART_BAUD                   9600
#define MCX_UART_TX                     UART4_TX_C16
#define MCX_UART_RX                     UART4_RX_C17

#define MCX_UART_INIT_TIME_OUT_MS       2000       
#define MCX_UART_INIT_WAKE_DATA         0x01

#define MCX_READ_BUFFER_LEN             64
#define MCX_BUFFER_HEAD                 0xFE
#define MCX_BUFFER_TAIL                 0xEF
#define MCX_DATA_LENGTH                 30

typedef enum _CubeDetectionState{
    CUBE_OUTSIDE_VIEW,
    CUBE_INSIDE_VIEW,
} CubeDetectionState;

typedef struct _CubeInfo{
    CubeDetectionState state;
    uint16 x_center;
    uint16 y_center;
    uint32 p_count;
} CubeInfo;

extern CubeInfo cube_info;


uint8 mcx_init_wait(void);
void  mcx_receive_data_callback(void);

void mcx_receive_data_interrupt_enable();
void mcx_receive_data_interrupt_disable();
void mcx_cube_data_handle_pit_call();
void mcx_uart_data_handle_pit_init();

#endif