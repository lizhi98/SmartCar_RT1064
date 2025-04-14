#include<stdlib.h>
#include "zf_driver_delay.h"

#include "menu.h"


MenuPageIndex current_page_index = MAIN_PAGE;  // 当前页面索引

// flag
uint8 menu_fresh_finish_flag = 0;  // 刷新标志位

MenuItem menu_items[] = {
    // ========================================= MENU PAGE =================================================
    {   .index = MAIN_START_ONLY,               .type = FUNCTION_MENU_ITEM,         .name = "Start Only", 
        .current_page_index = MAIN_PAGE,
        .data = {
            .function_menu_item_data = {.function_pointer = NULL}  // TODO: add function pointer
        }
    },
    {   .index = MAIN_START_WITH_INFO,          .type = FUNCTION_MENU_ITEM,         .name = "Start With Info", 
        .current_page_index = MAIN_PAGE,
        .data = {
            .function_menu_item_data = {.function_pointer = NULL}  // TODO: add function pointer
        } 
    },
    {   .index = MAIN_PARAMETER_ADJUST,         .type = PARENT_MENU_ITEM,           .name = "Parameter Adjust", 
        .current_page_index = MAIN_PAGE,
        .data = {
            .parent_menu_item_data = {.child_page_index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE}
        }
    },
    // ============================= PARAMETER_ADJUST_CLASS_CHOOSE_PAGE ====================================
    {   .index = PARAMETER_ADJUST_PID,          .type = PARENT_MENU_ITEM,           .name = "PID", 
        .current_page_index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,
        .data = {
            .parent_menu_item_data = {.child_page_index = PID_PAGE}
        }
    },
    {   .index = PARAMETER_ADJUST_SPEED,        .type = PARENT_MENU_ITEM,           .name = "Speed", 
        .current_page_index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,
        .data = {
            .parent_menu_item_data = {.child_page_index = SPEED_PAGE}
        }
    },
    // ======================================== PID_PAGE ==================================================
    {   .index = PID_ADJUST_KP,                 .type = PARAMETER_MENU_ITEM,        .name = "Kp", 
        .current_page_index = PID_PAGE,
        .data = {
            .parameter_menu_item_data = {.parameter_type = FLOAT_E, .parameter_pointer = NULL, .step_value_pointer = NULL}  // TODO: add parameter pointer and step value pointer
        }
    },
    {   .index = PID_ADJUST_KI,                 .type = PARAMETER_MENU_ITEM,        .name = "Ki", 
        .current_page_index = PID_PAGE,
        .data = {
            .parameter_menu_item_data = {.parameter_type = FLOAT_E, .parameter_pointer = NULL, .step_value_pointer = NULL}  // TODO: add parameter pointer and step value pointer
        }
    },
    {   .index = PID_ADJUST_KD,                 .type = PARAMETER_MENU_ITEM,        .name = "Kd", 
        .current_page_index = PID_PAGE,
        .data = {
            .parameter_menu_item_data = {.parameter_type = FLOAT_E, .parameter_pointer = NULL, .step_value_pointer = NULL}  // TODO: add parameter pointer and step value pointer
        }
    },
    // ======================================== SPEED_PAGE ==================================================
    {   .index = SPEED,                         .type = PARAMETER_MENU_ITEM,        .name = "Speed", 
        .current_page_index = SPEED_PAGE,
        .data = {
            .parameter_menu_item_data = {.parameter_type = INT32_E, .parameter_pointer = NULL, .step_value_pointer = NULL}  // TODO: add parameter pointer and step value pointer
        }
    },
    
};

MenuPage menu_pages[] = {
    {   .index = MAIN_PAGE,                             .name = "Main Menu",
        .start_index = MAIN_START_ONLY,                 .end_index = MAIN_PARAMETER_ADJUST,     .parent_page_index = NULL,
        .current_index_pointer = MAIN_START_ONLY
    },
    {   .index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,    .name = "Parameter Adjust",
        .start_index = PARAMETER_ADJUST_PID,            .end_index = PARAMETER_ADJUST_SPEED,    .parent_page_index = MAIN_PAGE,
        .current_index_pointer = PARAMETER_ADJUST_PID
    },
    {   .index = PID_PAGE,                              .name = "PID",
        .start_index = PID_ADJUST_KP,                   .end_index = PID_ADJUST_KD,             .parent_page_index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,
        .current_index_pointer = PID_ADJUST_KP
    },
    {   .index = SPEED_PAGE,                            .name = "Speed",                        .parent_page_index = PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,
        .start_index = SPEED,                           .end_index = SPEED, 
        .current_index_pointer = SPEED
    },
};

int menu_item_compare(const void *a, const void *b) {
    const MenuItem *item_a = (const MenuItem *)a;
    const MenuItem *item_b = (const MenuItem *)b;
    return item_a->index - item_b->index;
}

int menu_page_compare(const void *a, const void *b) {
    const MenuPage *page_a = (const MenuPage *)a;
    const MenuPage *page_b = (const MenuPage *)b;
    return page_a->index - page_b->index;
}

void menu_sort(void){
    qsort(menu_items, sizeof(menu_items)/sizeof(MenuItem), sizeof(MenuItem), menu_item_compare);
    qsort(menu_pages, sizeof(menu_pages)/sizeof(MenuPage), sizeof(MenuPage), menu_page_compare);
}

void menu_init(void){
    menu_sort();  // 按照index排序，使得数组的下标和index一致
    screen_full(RGB565_BLACK);  // 清屏
}

void menu_animation_block_jump_item(MenuItemIndex item_from,MenuItemIndex item_to, uint16 item_from_length,uint16 item_to_length){
    if(item_from == item_to) return;  // 如果没有跳转，则不需要创建补间动画
    // 首先计算item在屏幕的起始y坐标
    uint16 item_from_ys = (item_from - menu_pages[menu_items[item_from].current_page_index].start_index + 1u) * MENU_ITEM_WIDTH;
    uint16 item_to_ys   = (item_to -   menu_pages[menu_items[item_to]  .current_page_index].start_index + 1u) * MENU_ITEM_WIDTH;
    // 计算y步长，保证动画时间相同
    int8 y_step = (int8)((item_to_ys - item_from_ys) / MENU_ITEM_WIDTH);  // y步长
    // 然后计算方块x长度变化量相对于方块y坐标的变化率，即x步长
    int8 x_step = (int8)((item_to_length - item_from_length) / (item_to_ys - item_from_ys));  // x步长
    // 外层循环，循环方块的y起始位置：方块从开始运动到结束
    while(0);
}

void menu_fresh(MenuFreshMode mode){
    if(menu_fresh_finish_flag == 1) return;  // 如果不需要刷新，则不刷新
    switch(mode){
        case MENU_FRESH_FULL:
            screen_full(RGB565_BLACK);  // 清屏
            

            break;
        case MENU_FRESH_POINTER:
            break;
        default:
            break;
    }
}

void menu_key_pressed_event_handler(key_index_enum key){
    
}
