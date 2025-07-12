#include "screen.h"

char    ips200_show_info_str_buffer[64];
char *  ips200_show_info_ele_buffer;

char    ips200_show_cube_picture[64];



void screen_init(void){
    ips200pro_init(NULL, IPS200PRO_TITLE_BOTTOM, 20);
    ips200pro_set_format(IPS200PRO_FORMAT_UTF8);
    ips200pro_set_direction(IPS200PRO_PORTRAIT_180);
}
