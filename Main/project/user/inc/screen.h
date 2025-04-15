#ifndef _CODE_SCREEN_H_
#define _CODE_SCREEN_H_

#include "zf_device_ips200.h"
#include "zf_common_font.h"

#define screen_show_str(x,y,str)        ips200_show_string(x,y,str)
#define screen_show_int(x,y,dat,num)    ips200_show_int(x,y,dat,num)
#define screen_clear()                  ips200_clear()
#define screen_full(color)              ips200_full(color)

void screen_init(void);


#endif