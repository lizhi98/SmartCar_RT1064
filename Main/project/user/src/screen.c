#include "screen.h"

char    ips200_show_info_str_buffer[64];
char *  ips200_show_info_ele_buffer;

char    ips200_show_cube_picture[64];

char * cube_picture_class_name[] = {
    "wrench",		    // 扳手		
    "soldering_iron",	// 电烙铁		
    "electrodrill",	    // 电钻		
    "tape_measure",	    // 卷尺		
    "screwdriver",	    // 螺丝刀		
    "pliers",		    // 钳子		
    "oscillograph",	    // 示波器		
    "multimeter",	    // 万用表
    "printer",		    // 打印机		
    "keyboard",		    // 键盘		
    "mobile_phone",	    // 手机		
    "mouse",		    // 鼠标		
    "headphones",	    // 头戴式耳机	
    "monitor",		    // 显示器		
    "speaker",		    // 音响		
};

void screen_init(void){
    ips200pro_init("知行二队", IPS200PRO_TITLE_LEFT, 20);
}



