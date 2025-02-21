#include "mt_image.h"

// 返回值说明 0-成功 1-错误
uint8 mt_camera_init(void){
    return mt9v03x_init();
}