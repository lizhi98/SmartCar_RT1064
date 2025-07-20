#ifndef _CODE_MENU_H_
#define _CODE_MENU_H_

#include "math.h"

#include "zf_device_key.h"
#include "screen.h"
#include "OpenART.h"

// 立方体信息显示菜单页面

#define CUBE_INFO_PAGE_LIST_SIZE 13 // 立方体信息页面列表大小

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

// 当前页面ID
extern uint16 current_page_id; // 当前页面ID

void menu_init(void); // 菜单初始化
void cube_info_add(CubeFaceInfoClass class, uint8 number);
void cube_info_table_flash(void); // 刷新立方体信息表格
void debug_info_table_flash(void); // 刷新调试信息表格
void image_show_flash(void);

void change_page_to_debug_info(void);
void change_page_to_cube_info(void);
void change_page_to_image(void);

void key_change_page(key_index_enum key_n); // 按键切换页面
void key_select_next_table_row(key_index_enum key_change); // 按键选择下一个表格行

#endif