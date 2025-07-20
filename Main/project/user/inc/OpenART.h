#ifndef _CODE_OPEN_MV_H_
#define _CODE_OPEN_MV_H_

#include "stdint.h"

#include "zf_driver_uart.h"
#include "zf_driver_delay.h"
#include "zf_common_fifo.h"
#include "zf_driver_timer.h"
#include "zf_device_ips200.h"
#include "zf_driver_pit.h"

#define ART_UART_N                      UART_1
#define ART_UART_BAUD                   9600
#define ART_UART_TX                     UART1_TX_B12
#define ART_UART_RX                     UART1_RX_B13

#define ART_UART_INIT_TIME_OUT_MS       2000
#define ART_UART_INIT_WAKE_DATA         0x02

#define ART_READ_BUFFER_LEN             64
#define ART_BUFFER_HEAD                 '['
#define ART_BUFFER_TAIL                 ']'
#define ART_DATA_LENGTH                 24

typedef enum _CubePushDir{
    CUBE_PUSH_DIR_LEFT,
    CUBE_PUSH_DIR_RIGHT,
} CubePushDir;

typedef enum _CubeFaceInfoClass {
    electrodrill,       // 电钻
    headphones,         // 头戴式耳机
    keyboard,           // 键盘
    tape_measure,       // 卷尺
    mobile_phone,       // 手机
    monitor,            // 显示器
    mouse,              // 鼠标
    multimeter,         // 万用表
    number,             // 数字
    oscillograph,       // 示波器
    pliers,             // 钳子
    printer,            // 打印机
    screwdriver,        // 螺丝刀
    soldering_iron,     // 电烙铁
    speaker,            // 音响		
    wrench,             // 扳手		
} CubeFaceInfoClass;

extern char * cube_picture_class_name[];

typedef struct _CubeFaceInfo {
    uint8               valid; // 是否有效 1：有效 0：无效
    CubeFaceInfoClass   class; // 立方体类别
    float               class_prob; // 类别置信度
    uint8               number; // 数字
    float               number_prob; // 数字置信度
} CubeFaceInfo;

extern CubeFaceInfo current_cube_face_info; // 当前立方体上图片信息

char * get_name_of_cube_class(CubeFaceInfoClass class);

uint8 art_init(void);
void  art_receive_data_callback(void);

void art_receive_data_interrupt_enable();
void art_receive_data_interrupt_disable();
void art_cube_data_handle_pit_call();
void art_uart_data_handle_pit_init();

CubePushDir get_cube_push_dir(CubeFaceInfoClass class, uint8 number);

#endif