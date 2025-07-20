#include "screen.h"

void screen_init(void){
    ips200pro_init(NULL, IPS200PRO_TITLE_BOTTOM, 20);
    ips200pro_set_format(IPS200PRO_FORMAT_UTF8);
    ips200pro_set_direction(IPS200PRO_PORTRAIT_180);
}
