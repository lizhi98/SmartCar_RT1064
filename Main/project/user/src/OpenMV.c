#include "OpenMV.h"

vuint8 art_init_flag = 0; // art模块初始化标志位
vuint8 art_data_recv_finish_flag = 0; // art模块数据接收完成标志位

CubeFaceInfo current_cube_face_info = {
    .valid  = 0,
    .class  = electrodrill, // 默认类别
    .number = 0,             // 默认数字
    .class_prob = 0.0f,      // 默认类别置信度
    .number_prob = 0.0f      // 默认数字置信度
};

char * cube_picture_class_name[] = {
    "电钻",
    "头戴式耳机",
    "键盘",
    "卷尺",
    "手机",
    "显示器",
    "鼠标",
    "万用表",
    "数字",  // 要显示具体数字，不显示类别名称
    "示波器",
    "钳子",
    "打印机",
    "螺丝刀",
    "电烙铁",
    "音响",
    "扳手",
};

// art_Vision 数据缓冲和标志位
fifo_struct art_uart_fifo;

uint8        art_uart_fifo_buffer[ART_READ_BUFFER_LEN];
vuint8       art_uart_fifo_index = 0; // FIFO索引

uint8        art_data_buffer[ART_DATA_LENGTH];

vuint8       art_uart_interrupt_data_temp = 0; // 中断接收的单字节数据缓存

/*
* 函数简介     art模块UART通信初始化函数
* 参数说明     void
* 返回参数     0:成功 1:失败
* 使用示例     art_init();
* 备注信息     初始化art的串口,fifo和接收中断
*/
uint8 art_init(void) {
    // 初始化UART
    uart_init(ART_UART_N,ART_UART_BAUD,ART_UART_TX,ART_UART_RX);
    // 初始化FIFO
    // if(fifo_init(&art_uart_fifo, FIFO_DATA_8BIT, art_uart_fifo_buffer, ART_READ_BUFFER_LEN) != FIFO_SUCCESS) {
    //     return 1; // FIFO初始化失败
    // }
    art_init_flag = 1; // 设置初始化标志位
    return 0;
}

char * get_name_of_cube_class(CubeFaceInfoClass class) {
    if (class < 0 || class >= 16) { // sizeof(cube_picture_class_name) / sizeof(cube_picture_class_name[0]) is 16
        return "未知类别"; // 返回未知类别
    }
    return cube_picture_class_name[class];
}

void art_receive_data_callback(void) {
    uart_query_byte(ART_UART_N, (uint8 *)&art_uart_fifo_buffer[art_uart_fifo_index++]); // 读取串口数据
    if (art_uart_fifo_index >= ART_READ_BUFFER_LEN) { // 如果索引超过缓冲区长度
        art_uart_fifo_index = 0; // 重置索引
        return;
    }
    if (!art_init_flag || art_data_recv_finish_flag) return;
    // 处理接收到的数据
    if (art_uart_fifo_buffer[art_uart_fifo_index - 1] == ART_BUFFER_TAIL) { // 如果接收到数据尾
        art_data_recv_finish_flag = 1; // 设置数据接收完成标志位
    }else if (art_uart_fifo_buffer[art_uart_fifo_index - 1] == ART_BUFFER_HEAD) { // 如果接收到数据头
        art_data_recv_finish_flag = 0; // 设置数据接收开始标志位
        art_uart_fifo_index = 0; // 重置索引
    }
}

void art_receive_data_interrupt_enable(){
    uart_rx_interrupt(ART_UART_N, 1); // 打开接收中断
}

void art_receive_data_interrupt_disable(){
    uart_rx_interrupt(ART_UART_N, 0); // 关闭接收中断
}

void art_cube_data_handle_pit_call(){
    // 未初始化或数据接收未完成则return
    if(!art_init_flag || !(art_data_recv_finish_flag)){
        return;
    }
    // 添加\0结束符
    art_uart_fifo_buffer[art_uart_fifo_index - 1] = '\0'; // 添加结束符
    sscanf((char *)&art_uart_fifo_buffer[0],"o:%u,p:%f,n:%d,p:%f",
            &current_cube_face_info.class, &current_cube_face_info.class_prob, &current_cube_face_info.number, &current_cube_face_info.number_prob); // 解析数据
    current_cube_face_info.valid = 1; // 数据有效
    art_uart_fifo_index = 0; // 重置索引
    art_data_recv_finish_flag = 0;
}

void art_uart_data_handle_pit_init(){
    pit_ms_init(PIT_CH3, 400); // art_Vision 定时器初始化
}

