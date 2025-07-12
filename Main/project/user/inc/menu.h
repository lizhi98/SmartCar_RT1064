#ifndef _CODE_MENU_H_
#define _CODE_MENU_H_

#include "math.h"

#include "zf_device_key.h"
#include "screen.h"

// 立方体信息显示菜单页面

#define CUBE_INFO_PAGE_LIST_SIZE 12 // 立方体信息页面列表大小

typedef enum _CubeFaceInfoClass {
    CUBE_INFO_CLASS_NUMBER = 0, // 数字
    CUBE_INFO_CLASS_WRENCH, // 扳手
    CUBE_INFO_CLASS_SOLDERING_IRON, // 电烙铁
    CUBE_INFO_CLASS_DRILL, // 电钻
    CUBE_INFO_CLASS_TAPE_MEASURE, // 卷尺
    CUBE_INFO_CLASS_SCREWDRIVER, // 螺丝刀
    CUBE_INFO_CLASS_PLIERS, // 钳子
    CUBE_INFO_CLASS_OSCILLOSCOPE, // 示波器
    CUBE_INFO_CLASS_MULTIMETER, // 万用表
    CUBE_INFO_CLASS_PRINTER, // 打印机
    CUBE_INFO_CLASS_KEYBOARD, // 键盘
    CUBE_INFO_CLASS_PHONE, // 手机
    CUBE_INFO_CLASS_MOUSE, // 鼠标
    CUBE_INFO_CLASS_HEADSET, // 头戴式耳机
    CUBE_INFO_CLASS_MONITOR, // 显示器
    CUBE_INFO_CLASS_SPEAKER // 音响
} CubeFaceInfoClass;

typedef struct _CubeFaceInfo {
    uint8 valid; // 是否有效 1：有效 0：无效
    CubeFaceInfoClass class; // 立方体类别
    uint8 number; // 数字
} CubeFaceInfo;

extern uint16 cube_info_page_id;  // 立方体信息页面ID
extern uint16 cube_info_table_id; // 立方体信息表格ID

extern uint16       cube_info_list_next_index; // 立方体信息列表下一个索引
extern CubeFaceInfo cube_info_list[CUBE_INFO_PAGE_LIST_SIZE];

// 调试信息显示
extern uint16 debug_info_page_id; // 调试信息页面ID
extern uint16 debug_info_table_id; // 调试信息表格ID

// 图像显示
extern uint16 image_page_id; // 图像显示页面ID
extern uint16 image_image_id; // 图像显示组件ID

void menu_init(void); // 菜单初始化
void cube_info_add(CubeFaceInfoClass class, uint8 number);
void cube_info_table_flash(void); // 刷新立方体信息表格
void debug_info_table_flash(void); // 刷新调试信息表格
void image_show_flash(void);

void change_page_to_debug_info(void);
void change_page_to_cube_info(void);
void change_page_to_image(void);

#endif