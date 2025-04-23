#ifndef _CODE_MAIN_H_
#define _CODE_MAIN_H_

#include "stdio.h"

#include "correspond.h"
#include "motion_control.h"
#include "screen.h"
#include "gyroscope.h"
#include "mt_camera.h"
#include "icm42688.h"
#include "mt_image.h"
#include "menu.h"
#include "MCX_Vision.h"
#include "OpenMV.h"
#include "zf_common_headfile.h"

#define WIFI_SPI_SEND_INTERVAL 100
// 图像处理时间记录
extern uint32 image_process_time_start;
extern uint32 image_process_time;

extern vuint16 scc8660_image_buffer[SCC8660_H * SCC8660_W];

#endif