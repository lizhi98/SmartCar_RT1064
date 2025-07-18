#ifndef _CODE_MAIN_H_
#define _CODE_MAIN_H_

#include "stdio.h"

#include "zf_common_headfile.h"

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

// 图像处理时间记录
extern uint32 image_process_time;
extern uint32 image_process_wait_next_time;
extern uint8 zebra_valid_flag;

// void push_box();

#endif