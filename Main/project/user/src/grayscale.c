#include "grayscale.h"

uint8 grayscale_data = 1; // 灰度传感器数据

void grayscale_init(void){
    gpio_init(GRAYSCALE_PIN, GPI, GPIO_HIGH, GPI_PULL_UP); // 初始化灰度传感器引脚
}

void grayscale_read(){
    grayscale_data = gpio_get_level(GRAYSCALE_PIN); // 读取灰度传感器引脚状态
}
