#ifndef _CODE_MENU_H_
#define _CODE_MENU_H_

#include "math.h"

#include <zf_device_key.h>
#include <screen.h>

#define MENU_ITEM_WIDTH  20u

// =============================== INDEX ===============================
typedef enum _MenuPageIndex{
    MAIN_PAGE,
    PARAMETER_ADJUST_CLASS_CHOOSE_PAGE,
    PID_PAGE,
    SPEED_PAGE
}MenuPageIndex;

typedef enum _MenuItemIndex{
    // 主菜单菜单项目索引
    MAIN_START_ONLY,
    MAIN_START_WITH_INFO,
    MAIN_PARAMETER_ADJUST,
    // 调参菜单菜单项目索引
    PARAMETER_ADJUST_PID,
    PARAMETER_ADJUST_SPEED,
    // PID菜单菜单项目索引
    PID_ADJUST_KP,
    PID_ADJUST_KI,
    PID_ADJUST_KD,
    // SPEED菜单菜单项目索引
    SPEED,
}MenuItemIndex;

// =============================== MENU PAGE ===============================
typedef struct _MenuPage{
    MenuPageIndex index;                    // 菜单页面索引
    char * name;                            // 菜单页面名称
    MenuItemIndex start_index;              // 页面第一个菜单索引
    MenuItemIndex end_index;                // 页面最后一个菜单索引
    MenuPageIndex parent_page_index;         // 父页面索引
    MenuItemIndex current_index_pointer;    // 页面当前菜单索引
}MenuPage;

// =============================== MENU ITEM ===============================
typedef enum _MenuItemType{
    PARENT_MENU_ITEM,     // 父菜单项，即包含子菜单的菜单项目
    PARAMETER_MENU_ITEM,  // 参数菜单项，即参数调整的菜单项目
    FUNCTION_MENU_ITEM,   // 功能菜单项，即功能性的菜单项目
}MenuItemType;

// 参数类型
typedef enum _ParameterType{
    INT32_E,        // 整数类型
    FLOAT_E,        // 浮点数类型
    DOUBLE_E,       // 双精度浮点数类型
    UINT8_E,        // 无符号8位整数类型
}ParameterType;

// 要放在菜单数据union里的各个菜单类型的菜单数据结构体
typedef struct _ParentMenuItemData{
    MenuPageIndex child_page_index;     // 子页面索引
}ParentMenuItemData;
typedef struct _ParameterMenuItemData{
    ParameterType parameter_type;       // 参数类型
    void * parameter_pointer;           // 参数指针
    void * step_value_pointer;          // 步长值指针
}ParameterMenuItemData;
typedef struct _FunctionMenuItemData{
    void * function_pointer;            // 函数指针
}FunctionMenuItemData;

typedef union _MenuItemData{
    ParentMenuItemData      parent_menu_item_data;          // 父菜单项数据
    ParameterMenuItemData   parameter_menu_item_data;       // 参数菜单项数据
    FunctionMenuItemData    function_menu_item_data;        // 功能菜单项数据
}MenuItemData;

typedef struct _MenuItem{
    MenuItemIndex index;  // 菜单项目索引
    MenuItemType  type;   // 菜单项目类型
    char *        name;   // 菜单项目(显示)名称
    MenuPageIndex current_page_index; // 所在菜单页面索引
    MenuItemData  data;   // 菜单项目数据
}MenuItem;

// =============================== MENU FRESH ===============================
typedef enum _MenuFreshMode{
    MENU_FRESH_FULL,     // 全屏刷新
    MENU_FRESH_POINTER,  // 仅刷新指针
}MenuFreshMode;

extern MenuPageIndex current_page_index;  // 当前页面索引

void menu_sort(void);
void menu_init(void);

void menu_fresh(MenuFreshMode mode);

void menu_animation_block_jump_item(MenuItemIndex item_from,MenuItemIndex item_to, uint16 item_from_length,uint16 item_to_length);

void menu_animation_test(void);

// void menu_key_pressed_event_handler(key_index_enum key);

#endif