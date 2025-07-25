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

uint16          cube_info_list_next_index = 0;
CubeFaceInfo    cube_info_list[CUBE_INFO_PAGE_LIST_SIZE] = {0}; // 立方体信息列表，初始为空

// 调试信息显示
uint16 debug_info_page_id = 0; // 调试信息页面ID
uint16 debug_info_table_id = 0; // 调试信息表格ID
uint8  debug_info_page_heading_setted; // 是否设置了调试信息页面标题

// 图像显示
uint16 image_page_id = 0; // 图像显示页面ID
uint16 image_image_id = 0; // 图像显示组件ID

// 调参
uint16 param_page_id = 0; // 参数页面ID
uint16 param_table_id = 0; // 参数表格ID
uint8 param_table_selected_row = 0; // 参数表格选中行

typedef struct Param {
    const char *name;   // 参数名称

    enum ParamType {
        U8_PARAM,
        U16_PARAM,
        F32_PARAM,
        F64_PARAM,
        BOOL_PARAM,
    } type;

    union ParamInner {
        struct U8Param {
            uint8 *value;
            uint8 min;
            uint8 max;
            uint8 step;
        } u8_param;

        struct U16Param {
            uint16 *value;
            uint16 min;
            uint16 max;
            uint16 step;
        } u16_param;

        struct F32Param {
            float *value;
            float min;
            float max;
            float step;
        } f32_param;

        struct F64Param {
            double *value;
            double min;
            double max;
            double step;
        } f64_param;

        struct BoolParam {
            bool *value;
        } bool_param;
        
    } param;
} Param;

uint16 param_straight_pid_output = 80;
uint16 param_curve_pid_output = 70;
bool param_enable_loop = true;


Param params[] = {
    { .name = "直道输出", .type = U16_PARAM, .param.u16_param = { .value = &param_straight_pid_output, .min = 70, .max = 130, .step = 5 } },
    { .name = "曲线输出", .type = U16_PARAM, .param.u16_param = { .value = &param_curve_pid_output, .min = 60, .max = 120, .step = 5 } },
    { .name = "启用圆环", .type = BOOL_PARAM, .param.bool_param = { .value = &param_enable_loop } },
};
uint8 param_count = sizeof(params) / sizeof(Param);

void menu_init(void) {
    memset(cube_info_list, 0, sizeof(cube_info_list)); // 初始化立方体信息列表为全0

    // 创建页面和容器
    cube_info_page_id  = ips200pro_page_create("箱子显示"); // 创建立方体信息显示页面
    debug_info_page_id = ips200pro_page_create("调试信息"); // 创建调试信息显示页面
    image_page_id      = ips200pro_page_create("图像显示"); // 创建图像显示页面
    param_page_id      = ips200pro_page_create("调参"   );  // 创建调参页面
    
    uint16 cube_info_container_id  = ips200pro_container_create(0, 0, 240, 300); // 创建立方体容器
    uint16 debug_info_container_id = ips200pro_container_create(0, 0, 240, 300); // 创建调试信息容器
    uint16 image_container_id      = ips200pro_container_create(0, 0, 240, 300); // 创建图像显示容器
    uint16 param_container_id      = ips200pro_container_create(0, 0, 240, 300); // 创建调参容器

    // 设置各容器边线宽度为1，圆角半径为10
    ips200pro_container_radius(cube_info_container_id, 2, 10);
    ips200pro_container_radius(debug_info_container_id, 2, 10);
    ips200pro_container_radius(image_container_id, 2, 10);
    ips200pro_container_radius(param_container_id, 2, 10);
    
    cube_info_table_id  = ips200pro_table_create(0, 0, CUBE_INFO_PAGE_LIST_SIZE, 2);    // 创建立方体信息显示表格组件
    debug_info_table_id = ips200pro_table_create(0, 0, 12, 2);                          // 创建调试信息表格组件
    image_image_id = ips200pro_image_create(0, 0, 188, 120);                            // 创建图像显示组件
    param_table_id = ips200pro_table_create(0, 0, 12, 2);                               // 创建调参表格组件

    // 设置组件父子关系
    ips200pro_set_parent(cube_info_container_id,  cube_info_page_id);
    ips200pro_set_parent(debug_info_container_id, debug_info_page_id);
    ips200pro_set_parent(cube_info_table_id,  cube_info_container_id);
    ips200pro_set_parent(debug_info_table_id, debug_info_container_id);
    ips200pro_set_parent(image_image_id, image_container_id);

    // 设置表格列宽
    ips200pro_table_set_col_width(cube_info_table_id,  1, 80);
    ips200pro_table_set_col_width(cube_info_table_id,  2, 140);
    ips200pro_table_set_col_width(debug_info_table_id, 1, 80); 
    ips200pro_table_set_col_width(debug_info_table_id, 2, 140);
    ips200pro_table_set_col_width(param_table_id, 1, 100);
    ips200pro_table_set_col_width(param_table_id, 2, 100);

    current_page_id = image_page_id; // 设置当前页面为图像显示
}

/**
 * @brief 添加下一个立方体的信息
 * @param class 立方体信息类别
 * @param number 若立方体为标数字的立方体，则此参数为具体数字
 * @note 如果列表已满，则不添加
 */
void cube_info_add(CubeFaceInfoClass class, uint8 number) {
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
void cube_info_table_refresh() {
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

void change_page_to_image() {
    ips200pro_page_switch(image_page_id, PAGE_ANIM_OFF); // 切换到图像显示页面
    current_page_id = image_page_id; // 设置当前页面为图像显示页面
}

void change_page_to_param() {
    ips200pro_page_switch(param_page_id, PAGE_ANIM_OFF); // 切换到参数页面
    current_page_id = param_page_id; // 设置当前页面为参数页面
}

void debug_info_table_refresh() {
    if (debug_info_table_id == 0 || current_page_id != debug_info_page_id) {
        return; // 如果表格ID为0，说明没有初始化，则不进行刷新
    }
    if (!debug_info_page_heading_setted) {
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
    ips200pro_table_cell_printf(debug_info_page_id,  9, 2, "%u %u %u", cube_info.state, cube_info.x_length, mcx_data_time); // 显示 MCX_Vision 接收到的立方体大小(像素数量)
    ips200pro_table_cell_printf(debug_info_page_id, 10, 2, "%u", grayscale_data); // 显示灰度传感器数据
}

void param_table_refresh() {
    for (int i = 0; i < param_count; i++) {
        Param *p_param = &params[i];
        ips200pro_table_cell_printf(param_table_id, i + 1, 1, "%s", p_param->name); // 显示参数名称
        switch (p_param->type) {
            case U16_PARAM:
                ips200pro_table_cell_printf(param_table_id, i + 1, 2, "%u", *p_param->param.u16_param.value);
                break;
            case U8_PARAM:
                ips200pro_table_cell_printf(param_table_id, i + 1, 2, "%u", *p_param->param.u8_param.value);
                break;
            case F32_PARAM:
                ips200pro_table_cell_printf(param_table_id, i + 1, 2, "%.2f", *p_param->param.f32_param.value);
                break;
            case F64_PARAM:
                ips200pro_table_cell_printf(param_table_id, i + 1, 2, "%.2f", *p_param->param.f64_param.value);
                break;
            case BOOL_PARAM:
                ips200pro_table_cell_printf(param_table_id, i + 1, 2, "%s", *p_param->param.bool_param.value ? "True" : "False");
                break;
        }
    }
}

void image_show_refresh() {
    if (image_page_id == 0 || current_page_id != image_page_id) {
        return; // 如果页面ID为0，说明没有初始化，则不进行刷新
    }
    ips200pro_image_display(image_image_id, image_buffer, 188, 120, IMAGE_GRAYSCALE, 0); // 显示图像
}

void key_change_page(key_index_enum key) {
    if (key_get_state(key) == KEY_RELEASE) return;

    // 按顺序切换页面
    if (current_page_id == image_page_id)
        change_page_to_debug_info();    // 图像显示页面->调试信息页面
    else if (current_page_id == debug_info_page_id)
        change_page_to_cube_info();     // 调试信息页面->立方体信息页面
    else if (current_page_id == cube_info_page_id)
        change_page_to_param();         // 立方体信息页面->调参页面
    else
        change_page_to_image();         // 调参页面->图像显示页面

    // 清除按键状态
    key_clear_state(key);
}

void key_select_next_table_row(key_index_enum key) {
    if (key_get_state(key) == KEY_RELEASE) return;

    if (current_page_id == cube_info_page_id) {
        if (cube_info_table_selected_row ++ == CUBE_INFO_PAGE_LIST_SIZE) cube_info_table_selected_row = 1;
        ips200pro_table_select(cube_info_table_id, cube_info_table_selected_row, 1); // 选中下一行的表头
    } else if (current_page_id == param_page_id) {
        if (param_table_selected_row ++ == param_count) param_table_selected_row = 1;
        ips200pro_table_select(param_table_id, param_table_selected_row, 2); // 选中下一行的数值
    }

    // 清除按键状态
    key_clear_state(key);
}

void key_switch_motor(key_index_enum key) {
    if (key_get_state(key) == KEY_RELEASE) return;

    motor_enable_flag = !motor_enable_flag; // 切换电机使能状态

    // 清除按键状态
    key_clear_state(key);
}

#define CLAMP(_value, _min, _max) \
    do { \
        if ((_value) < (_min)) (_value) = (_min); \
        else if ((_value) > (_max)) (_value) = (_max); \
    } while (0)

void key_adjust_param(key_index_enum key_plus, key_index_enum key_minus) {
    bool key_plus_pressed = key_get_state(key_plus);
    bool key_minus_pressed = key_get_state(key_minus);
    if (key_plus_pressed == key_minus_pressed) return; // 如果两个按键都没有按下或都按下，则不进行调整

    Param *p_param = &params[param_table_selected_row - 1]; // 获取当前选中的参数
    int8 sign = key_plus_pressed ? 1 : -1; // 根据按键确定增减方向

    switch (p_param->type) {
        case U16_PARAM:
            *p_param->param.u16_param.value += sign * p_param->param.u16_param.step;
            CLAMP(*p_param->param.u16_param.value, p_param->param.u16_param.min, p_param->param.u16_param.max);
            break;
        case U8_PARAM:
            *p_param->param.u8_param.value += sign * p_param->param.u8_param.step;
            CLAMP(*p_param->param.u8_param.value, p_param->param.u8_param.min, p_param->param.u8_param.max);
            break;
        case F32_PARAM:
            *p_param->param.f32_param.value += sign * p_param->param.f32_param.step;
            CLAMP(*p_param->param.f32_param.value, p_param->param.f32_param.min, p_param->param.f32_param.max);
            break;
        case F64_PARAM:
            *p_param->param.f64_param.value += sign * p_param->param.f64_param.step;
            CLAMP(*p_param->param.f64_param.value, p_param->param.f64_param.min, p_param->param.f64_param.max);
            break;
        case BOOL_PARAM:
            *p_param->param.bool_param.value = ! *p_param->param.bool_param.value;
            break;
    }
    
     // 清除按键状态
    key_clear_state(key_plus);
    key_clear_state(key_minus);
}