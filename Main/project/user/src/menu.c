#include "zf_driver_delay.h"
#include "string.h"
#include "menu.h"
#include "motion_control_new.h"
#include "mt_image.h"
#include "gyroscope.h"
#include "grayscale.h"

uint16 current_page_id = 0; // 当前页面ID

// 立方体信息显示
uint16 cube_info_page_id = 0; // 立方体信息页面ID
uint16 cube_info_table_id = 0; // 立方体信息表格ID
uint8  cube_info_table_selected_row = 1; // 立方体信息表格选中行
// uint8  cube_info_table_selected_col = 1; // 立方体信息表格选中列

uint16          cube_info_list_next_index = 0;
CubeFaceInfo    cube_info_list[CUBE_INFO_PAGE_LIST_SIZE] = {0}; // 立方体信息列表，初始为空

// 调试信息显示
uint16 debug_info_page_id = 0; // 调试信息页面ID
uint16 debug_info_table_id = 0; // 调试信息表格ID
uint8  debug_info_page_heading_setted = 0; // 是否设置了调试信息页面标题

// 图像显示
uint16 image_page_id = 0; // 图像显示页面ID
uint16 image_image_id = 0; // 图像显示组件ID

void menu_init(void) {
    memset(cube_info_list, 0, sizeof(cube_info_list)); // 初始化立方体信息列表为全0
    // 创建页面和容器
    cube_info_page_id  = ips200pro_page_create("箱子显示");  // 创建立方体信息显示页面
    debug_info_page_id = ips200pro_page_create("调试信息");  // 创建调试信息显示页面
    image_page_id      = ips200pro_page_create("图像显示");   // 创建图像显示页面
    
    uint16 cube_info_container_id  = ips200pro_container_create(0, 0, 240, 300);  // 创建容器组件
    uint16 debug_info_container_id = ips200pro_container_create(0, 0, 240, 300); // 创建调试信息容器组件
    uint16 image_container_id      = ips200pro_container_create(0, 0, 240, 300); // 创建图像显示容器组件

    ips200pro_container_radius(cube_info_container_id, 2, 10);  // 设置容器边线宽度为1，圆角半径为10
    ips200pro_container_radius(debug_info_container_id, 2, 10); // 设置调试信息容器边线宽度为1，圆角半径为10
    ips200pro_container_radius(image_container_id, 2, 10);     // 设置图像显示容器边线宽度为1，圆角半径为10
    
    ips200pro_set_parent(cube_info_container_id,  cube_info_page_id);   // 将容器设置为立方体信息页面的父组件
    ips200pro_set_parent(debug_info_container_id, debug_info_page_id); // 将调试信息容器设置为调试信息页面的父组件

    // 创建表格组件
    cube_info_table_id  = ips200pro_table_create(0, 0, CUBE_INFO_PAGE_LIST_SIZE, 2); // 创建立方体信息显示表格组件
    debug_info_table_id = ips200pro_table_create(0, 0, 12, 2); // 创建调试信息表格组件
    image_image_id = ips200pro_image_create(0, 0, 188, 120); // 创建图像显示组件

    ips200pro_set_parent(cube_info_table_id,  cube_info_container_id); // 将立方体信息表格设置为立方体信息容器的子组件
    ips200pro_set_parent(debug_info_table_id, debug_info_container_id); // 将调试信息表格设置为调试信息容器的子组件
    ips200pro_set_parent(image_image_id, image_container_id); // 将图像显示组件设置为图像显示容器的子组件

    // 设置表格宽度
    ips200pro_table_set_col_width(cube_info_table_id,  1, 80);  // 设置第一列宽度为80
    ips200pro_table_set_col_width(cube_info_table_id,  2, 140); // 设置第二列宽度为140
    ips200pro_table_set_col_width(debug_info_table_id, 1, 80);  // 设置第一列宽度为50
    ips200pro_table_set_col_width(debug_info_table_id, 2, 140); // 设置第二列宽度为150

    current_page_id = image_image_id; // 设置当前页面为图像显示
}

/**
    * @brief 添加下一个立方体的信息
    * @param class 立方体信息类别
    * @param number 若立方体为标数字的立方体，则此参数为具体数字
    * @note 如果列表已满，则不添加
*/
void cube_info_add(CubeFaceInfoClass class, uint8 number){
    if (cube_info_list_next_index >= CUBE_INFO_PAGE_LIST_SIZE) {
        return; // 列表已满
    }
    cube_info_list[cube_info_list_next_index].valid  = 1; // 设置为有效
    cube_info_list[cube_info_list_next_index].class  = class; // 设置立方体类别
    cube_info_list[cube_info_list_next_index].number = number; // 设置数字
    cube_info_list_next_index++;
}

/**
 * @brief 刷新立方体信息表格
 * @note 将立方体信息表格中的内容刷新为当前的立方体信息列表
 *       立方体类别显示在第一列，数字或类别名称显示在第二列
 *       第一行是标题行，从第二行开始显示立方体信息
 */
void cube_info_table_flash(){
    if (cube_info_table_id == 0) {
        return; // 如果表格ID为0，说明没有初始化，则不进行刷新
    }
    for (uint16 i = 0; i < cube_info_list_next_index; i++) {
        // 判断信息是否为有效信息
        if (cube_info_list[i].valid == 0) {
            // 显示为空
            ips200pro_table_cell_printf(cube_info_table_id, i + 1, 1, " "); // 显示无效信息
            ips200pro_table_cell_printf(cube_info_table_id, i + 1, 2, " "); // 显示无效信息
            continue; // 继续
        }
        ips200pro_table_cell_printf(cube_info_table_id, i + 1, 1, "%u", i + 1); // 显示立方体类别
        // 判断是否为数字
        if (cube_info_list[i].class == number) {
            ips200pro_table_cell_printf(cube_info_table_id, i + 1, 2, "%u", cube_info_list[i].number); // 显示数字
        } else {
            ips200pro_table_cell_printf(cube_info_table_id, i + 1, 2, "%s", cube_picture_class_name[cube_info_list[i].class]); // 显示类别名称
        }
    }
}

void change_page_to_debug_info() {
    ips200pro_page_switch(debug_info_page_id, PAGE_ANIM_OFF); // 切换到指定页面
    current_page_id = debug_info_page_id; // 设置当前页面为调试信息页面
}

void change_page_to_cube_info() {
    ips200pro_page_switch(cube_info_page_id, PAGE_ANIM_OFF); // 切换到指定页面
    current_page_id = cube_info_page_id; // 设置当前页面为立方体信息页面
}

void change_page_to_image(void){
    ips200pro_page_switch(image_page_id, PAGE_ANIM_OFF); // 切换到图像显示页面
    current_page_id = image_page_id; // 设置当前页面为图像显示页面
}

void debug_info_table_flash() {
    if (debug_info_table_id == 0 || current_page_id != debug_info_page_id) {
        return; // 如果表格ID为0，说明没有初始化，则不进行刷新
    }
    if (!debug_info_page_heading_setted){
        ips200pro_table_cell_printf(debug_info_table_id, 1, 1, "速度");
        ips200pro_table_cell_printf(debug_info_page_id,  2, 1, "占空比%%");
        ips200pro_table_cell_printf(debug_info_page_id,  3, 1, "Offset");
        ips200pro_table_cell_printf(debug_info_page_id,  4, 1, "陀螺仪");
        ips200pro_table_cell_printf(debug_info_page_id,  5, 1, "图像时间");
        ips200pro_table_cell_printf(debug_info_page_id,  6, 1, "赛道元素");
        ips200pro_table_cell_printf(debug_info_page_id,  7, 1, "运动模式");
        ips200pro_table_cell_printf(debug_info_page_id,  8, 1, "ART");
        ips200pro_table_cell_printf(debug_info_page_id,  9, 1, "MCX");
        ips200pro_table_cell_printf(debug_info_page_id, 10, 1, "灰度");
        debug_info_page_heading_setted = 1; // 设置了标题
    }
    ips200pro_table_cell_printf(debug_info_table_id, 1, 2, "%3d %3d %3d", motors[0].current_speed, motors[1].current_speed, motors[2].current_speed); // 显示电机速度
    ips200pro_table_cell_printf(debug_info_page_id,  2, 2, "%3d %3d %3d", motors[0].pwm_duty / 100, motors[1].pwm_duty / 100, motors[2].pwm_duty / 100);
    ips200pro_table_cell_printf(debug_info_page_id,  3, 2, "%f", image_result.offset);
    ips200pro_table_cell_printf(debug_info_page_id,  4, 2, "%6.1f %5.0f", gyroscope_result.gyro_z, gyroscope_result.angle_z); // 显示陀螺仪数据);
    ips200pro_table_cell_printf(debug_info_page_id,  5, 2, "w:%u p:%u", image_process_wait_next_time, image_process_time);
    ips200pro_table_cell_printf(debug_info_page_id,  6, 2, "%u", image_result.element_type); // 显示赛道元素类型
    ips200pro_table_cell_printf(debug_info_page_id,  7, 2, "%u", motion_control.motion_mode); // 显示运动模式
    ips200pro_table_cell_printf(debug_info_page_id,  8, 2, "%s %u", get_name_of_cube_class(current_cube_face_info.class), current_cube_face_info.number); // 显示 OpenART 接收数据计数
    ips200pro_table_cell_printf(debug_info_page_id,  9, 2, "%u %u %u", cube_info.state, cube_info.p_count, mcx_data_time); // 显示 MCX_Vision 接收到的立方体大小(像素数量)
    ips200pro_table_cell_printf(debug_info_page_id, 10, 2, "%u", grayscale_data); // 显示灰度传感器数据
}

void image_show_flash() {
    if (image_page_id == 0 || current_page_id != image_page_id) {
        return; // 如果页面ID为0，说明没有初始化，则不进行刷新
    }
    ips200pro_image_display(image_image_id, image_buffer, 188, 120, IMAGE_GRAYSCALE, 0); // 显示图像
}

void key_change_page(key_index_enum key_n) {
    if (key_get_state(key_n) != KEY_RELEASE) {
        // 按顺序切换页面
        if (current_page_id == image_page_id) {
            change_page_to_debug_info(); // 切换到调试信息页面
        } else if (current_page_id == debug_info_page_id) {
            change_page_to_cube_info(); // 切换到立方体信息页面
        } else {
            change_page_to_image(); // 切换到图像显示页面
        }
        // 复原按键状态
        key_clear_state(key_n); // 清除按键状态
    }
}

void key_select_next_table_row(key_index_enum key_change) {
    if (key_get_state(key_change) != KEY_RELEASE){
        if (current_page_id == cube_info_page_id) {
            if (cube_info_table_selected_row > CUBE_INFO_PAGE_LIST_SIZE || cube_info_table_selected_row <= 0) {
                cube_info_table_selected_row = 1; // 回到第一行
            }
            ips200pro_table_select(cube_info_table_id, cube_info_table_selected_row, 1); // 设置选中行
            cube_info_table_selected_row++;
        } 
        // else if (current_page_id == debug_info_page_id) {
        //     debug_info_table_selected_row++;
        //     if (debug_info_table_selected_row >= 12) {
        //         debug_info_table_selected_row = 1; // 回到第一行
        //     }
        //     ips200pro_table_select(debug_info_table_id, debug_info_table_selected_row, 0x00); // 设置选中行
        // }
        // 复原按键状态
        key_clear_state(key_change); // 清除按键状态
    }
}
