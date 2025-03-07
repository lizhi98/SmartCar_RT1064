#include "correspond.h"


// ================WIFI SPI====================
// 函数简介     WiFi 模块初始化
// 参数说明     uint16 time_out_s  超时时间 单位s 尝试周期5s
// 返回参数     uint8           模块初始化状态 0-成功 1-错误

uint8 wifi_spi_init_(){
    return wifi_spi_init(WIFI_SSID,WIFI_PASS);
    // 先尝试一次
    // if(        (!wifi_spi_init(WIFI_SSID,WIFI_PASS)) 
    //         && (!wifi_spi_socket_connect(WIFI_SPI_CONNECT_MODE,WIFI_SPI_TARGET_IP,WIFI_SPI_TARGET_PORT,WIFI_SPI_LOCAL_PORT))
    // )   {return 0;}
    // else{return 1;}
    // // 启动定时器
    // timer_init(GPT_TIM_1,TIMER_MS);
    // // 连接WIFI
    // while (wifi_spi_init(WIFI_SSID,WIFI_PASS))
    // {
    //     system_delay_ms(5000);
    //     if(timer_get(GPT_TIM_1) >= ( time_out_s * 1000 ) ){
    //         timer_clear(GPT_TIM_1);
    //         timer_stop(GPT_TIM_1);
    //         return 1;
    //     }
    // }
    // // 建立与上位机连接
    // while (wifi_spi_socket_connect("TCP","192.168.137.1","9894","6060"))
    // {
    //     system_delay_ms(5000);
    //     if(timer_get(GPT_TIM_1) >= ( time_out_s * 1000 ) ){
    //         timer_clear(GPT_TIM_1);
    //         timer_stop(GPT_TIM_1);
    //         return 1;
    //     }
    // }
    // timer_clear(GPT_TIM_1);
    // timer_stop(GPT_TIM_1);
    // return 0;
}

// 函数简介     读取WIFI SPI的数据
// 参数说明     dst         要存入变量的地址
// 参数说明     dst_size_n_uint8    变量的字节数
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read(void * dst,uint32 dst_size_n_uint8){
    return (wifi_spi_read_buffer((uint8 *)dst,dst_size_n_uint8) != dst_size_n_uint8);
}


// ====================UART TO OpenART mini====================
void uart_init_(void){
    uart_init(UART_N,UART_BAUD,UART_TX_PIN,UART_RX_PIN);
}


