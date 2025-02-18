#include "zf_device_wifi_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_delay.h"

#define WIFI_SSID "AP_SmartCar"
#define WIFI_PASS "SMARTCAR"

uint8 * TO_HOST_CMD_HEAD = "THCS";
uint8 * TO_HOST_CMD_TAIL = "THCE";
uint8 * TO_ART_CMD_HEAD = "TOCS";
uint8 * TO_ART_CMD_TAIL = "TOCE";

uint8 * TO_HOST_DATA_HEAD = "THDS";
uint8 * TO_HOST_DATA_TAIL = "THDE";
uint8 * TO_ART_DATA_HEAD = "TODS";
uint8 * TO_ART_DATA_TAIL = "TODE";

// 函数简介     WiFi 模块初始化
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
uint8 wifi_spi_init_(uint16 time_out_s){
    if( wifi_spi_init(WIFI_SSID,WIFI_PASS) ){
        timer_init(GPT_TIM_1,TIMER_MS);
        while (wifi_spi_init(WIFI_SSID,WIFI_PASS))
        {
            system_delay_ms(2000);
            if(timer_get >= time_out_s * 1000){
                timer_stop(GPT_TIM_1);
                timer_clear(GPT_TIM_1);
                return 1;
            }
        }
    }
    return 0;
}


