#include "screen.h"

char    ips200_show_info_str_buffer[64];
char *  ips200_show_info_ele_buffer;

char    ips200_show_cube_picture[64];

void screen_init(void){
    ips200_init(IPS200_TYPE_SPI);
}

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

void screen_show_info(){
    uint16 y = 0; // 当前显示指针y坐标

    ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, otsu_threshold);
    y += MT9V03X_H + 20; // 更新y坐标

    switch(image_result.element_type){
        case Zebra:             ips200_show_info_ele_buffer = "Zebra  "; break;
        case CurveLeft:         ips200_show_info_ele_buffer = "CurveL "; break;
        case CurveRight:        ips200_show_info_ele_buffer = "CurveR "; break;
        case Normal:            ips200_show_info_ele_buffer = "Normal "; break;
        case CrossBefore:       ips200_show_info_ele_buffer = "CrossB "; break;
        case Cross:             ips200_show_info_ele_buffer = "Cross  "; break;
        case LoopLeftBefore:    ips200_show_info_ele_buffer = "LoopLB "; break;
        case LoopRightBefore:   ips200_show_info_ele_buffer = "LoopRB "; break;
        case LoopLeftBefore2:   ips200_show_info_ele_buffer = "LoopLB2"; break;
        case LoopRightBefore2:  ips200_show_info_ele_buffer = "LoopRB2"; break;
        case LoopLeft:          ips200_show_info_ele_buffer = "LoopL  "; break;
        case LoopRight:         ips200_show_info_ele_buffer = "LoopR  "; break;
        case LoopLeftAfter:     ips200_show_info_ele_buffer = "LoopLA "; break;
        case LoopRightAfter:    ips200_show_info_ele_buffer = "LoopRA "; break;
        case RampLeft:          ips200_show_info_ele_buffer = "RampL  "; break;
        case RampRight:         ips200_show_info_ele_buffer = "RampR  "; break;
        default:                ips200_show_info_ele_buffer = "Unknown"; break;
    }
    
    // sprintf(ips200_show_info_str_buffer, "Ele:%s", ips200_show_info_ele_buffer);
    // ips200_show_string(0, y, ips200_show_info_str_buffer); // 显示元素类型
    // sprintf(ips200_show_info_str_buffer, "offset:%7.1f", image_result.offset);
    // ips200_show_string(IPS200_X_MAX / 2, y, ips200_show_info_str_buffer); // 显示偏移量
    // y += 20; // 更新y坐标

    // sprintf(ips200_show_info_str_buffer, "fSpeed:%4d", translation_pid.front_speed_out);
    // ips200_show_string(0, y, ips200_show_info_str_buffer); // 显示前进速度
    // sprintf(ips200_show_info_str_buffer, "lSpeed:%4d", translation_pid.left_speed_out);
    // ips200_show_string(IPS200_X_MAX / 2, y, ips200_show_info_str_buffer); // 显示左y移速度

}

void screen_show_cube_picture(void){
    screen_clear();
    uint16 y = 0; // 当前显示指针y坐标
    
    
}

