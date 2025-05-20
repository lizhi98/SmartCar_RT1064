#ifndef _CODE_SCREEN_H_
#define _CODE_SCREEN_H_

#include "zf_device_ips200.h"
#include "zf_common_font.h"
#include "zf_device_mt9v03x.h"

#include "mt_image.h"
#include "motion_control.h"

#define IPS200_X_MAX 240
#define IPS200_Y_MAX 320

#define screen_show_str(x,y,str)        ips200_show_string(x,y,str)
#define screen_show_int(x,y,dat,num)    ips200_show_int(x,y,dat,num)
#define screen_clear()                  ips200_clear()
#define screen_full(color)              ips200_full(color)

void screen_init(void);
void screen_show_info(void);


#endif