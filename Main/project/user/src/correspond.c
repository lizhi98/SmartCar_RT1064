#include "correspond.h"


// ================WIFI SPI====================
// 函数简介     WiFi 模块初始化
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
uint8 wifi_spi_init_(uint16 time_out_s){
    if( wifi_spi_init(WIFI_SSID,WIFI_PASS) ){
        timer_init(GPT_TIM_1,TIMER_MS);
        while (wifi_spi_init(WIFI_SSID,WIFI_PASS))
        {
            system_delay_ms(2000);
            if(timer_get >= time_out_s * 1000){
                timer_clear(GPT_TIM_1);
                timer_stop(GPT_TIM_1);
                return 1;
            }
        }
    }
    return 0;
}

// ====================UART TO OpenART mini====================
void uart_init_(void){
    uart_init(UART_N,UART_BAUD,UART_TX_PIN,UART_RX_PIN);
}


