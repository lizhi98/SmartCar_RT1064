#include "zf_common_headfile.h"
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完
// 本例程是开源库移植用空工程
#include "main.h"

// 图像处理时间记录
uint32 image_process_time_start = 0;
uint32 image_process_time = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化
    // =====================外设初始化开始=========================
    // 屏幕初始化
    screen_init();
    ips200_clear();  // 清屏
    // ips200_draw_line(0,0,239,0,RGB565_RED);  // 画线测试
    // 如果程序在输出了断言信息 并且提示出错位置在这里
    // 那么一般是屏幕显示的时候超过屏幕分辨率范围了

    // ips200_draw_region(20, 30, 100, 300, RGB565_RED);  // 画区域测试
    // menu_animation_test();
    menu_init();  // 菜单初始化 MAIN_START_ONLY
    ips200_draw_region(0, 20, 150, 40, RGB565_WHITE);  // 画区域测试
    menu_animation_block_jump_item(MAIN_START_ONLY, MAIN_PARAMETER_ADJUST, 150, 100);  // 测试补间动画
    // =====================外设初始化结束========================
    // 主循环准备
    // 主循环
    while(1) {
        
    }
}
// void menu_animation_block_jump_item1(MenuItemIndex item_from,MenuItemIndex item_to, uint16 item_from_length,uint16 item_to_length){
//     if(item_from == item_to) return;  // 如果没有跳转，则不需要创建补间动画
//     // 首先计算item在屏幕的起始y坐标
//     uint16 item_from_ys = (item_from - menu_pages[menu_items[item_from].current_page_index].start_index + 1u) * MENU_ITEM_WIDTH;
//     uint16 item_to_ys   = (item_to -   menu_pages[menu_items[item_to]  .current_page_index].start_index + 1u) * MENU_ITEM_WIDTH;

//     uint16 item_d_y = item_to_ys        - item_from_ys;  // y坐标差值
//     uint16 item_d_x = item_to_length    - item_from_length;  // x坐标差值
//     // 定义循环次数，在该循环次数内完成补间动画
//     uint16 loop_times = 30;  // 循环次数
//     if(item_d_y > 0){
//         while(1);
//         double k = 0.0;
//         for(uint16 time = 0; time <= loop_times; time++){
//             double k_next = 1.0 - pow(1 - (double) time / (loop_times * 1.0) , 3.0);
//             system_delay_ms(30);  // 延时
    
//             ips200_draw_region(0, item_from_ys + k * item_d_y,      100, item_from_ys + k_next * item_d_y,                     RGB565_BLACK);  // 画区域测试
//             ips200_draw_region(0, item_from_ys + k_next * item_d_y, 100, item_from_ys + k_next * item_d_y + MENU_ITEM_WIDTH,   RGB565_WHITE);  // 画区域测试
    
//             k = k_next;
//         }
//     }else{
//         item_from_ys += MENU_ITEM_WIDTH;  // 如果是向上跳转，则需要加上一个菜单项的高度
//         item_to_ys   += MENU_ITEM_WIDTH;  // 如果是向上跳转，则需要加上一个菜单项的高度
//         double k = 0.0;
//         for(uint16 time = 0; time <= loop_times; time++){
//             double k_next = 1.0 - pow(1 - (double) time / (loop_times * 1.0) , 3.0);
//             system_delay_ms(30);  // 延时
    
//             ips200_draw_region(0, item_from_ys + k_next * item_d_y, 100, item_from_ys + k * item_d_y,                           RGB565_BLACK);  // 画区域测试
//             // while(1);
//             ips200_draw_region(0, item_from_ys + k_next * item_d_y - MENU_ITEM_WIDTH, 100, item_from_ys + k_next * item_d_y,    RGB565_WHITE);  // 画区域测试

//             k = k_next;
//         }
//     }
    
    
//     // 外层循环，循环方块的y起始位置：方块从开始运动到结束
//     while(1);
// }


