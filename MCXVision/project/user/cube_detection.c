#include "cube_detection.h"
CubeInfo cube_info = {.state = CUBE_OUTSIDE_VIEW ,.x_offset = 0, .y_offset = 0}; // 立方体信息结构体初始化


void find_red_cube_center(uint16_t *scc8660_image, int *x, int *y) {
    int x_min = SCC8660_W, x_max = 0;
    int y_min = SCC8660_H, y_max = 0;
    uint32_t pixel_count = 0;

    // 优化指针访问
    uint16_t *pixel_ptr = scc8660_image;
    
    for (int cy = 0; cy < SCC8660_H; cy++) {
        for (int cx = 0; cx < SCC8660_W; cx++) {
            uint16_t pixel = *pixel_ptr++;
            
            // 提取RGB分量
            uint8_t r = (pixel >> 11) & 0x1F;  // 5-bit
            uint8_t g = (pixel >> 5)  & 0x3F;  // 6-bit
            uint8_t b = pixel & 0x1F;          // 5-bit

            // 动态红色检测条件
            if (r > R_BASE_THRESHOLD &&         // 绝对阈值
               (r << 1) > g &&                 // R分量显著大于G（考虑位宽差异）
               (r > (b << 1))) {               // R分量显著大于B
                // 更新边界
                if (cx < x_min) x_min = cx;
                if (cx > x_max) x_max = cx;
                if (cy < y_min) y_min = cy;
                if (cy > y_max) y_max = cy;
                pixel_count++;
            }
        }
    }

    // 有效性检查
    if (pixel_count > MIN_RED_PIXELS && 
        x_max > x_min && 
        y_max > y_min) {
        *x = (x_min + x_max) / 2;
        *y = (y_min + y_max) / 2;
    } else {
        *x = -1;  // 无效坐标
        *y = -1;
    }
}

/* 使用示例：
int main() {
    int center_x, center_y;
    find_red_cube_center(scc8660_image, &center_x, &center_y);
    
    if (center_x != -1) {
        // 使用检测到的坐标
        // printf("Center at: (%d, %d)\n", center_x, center_y);
    }
    return 0;
}
*/