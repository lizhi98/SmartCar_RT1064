#include "cube_detection.h"

// 立方体信息初始化
CubeInfo cube_info = {
    .state = CUBE_OUTSIDE_VIEW,
    .x_offset = 0,
    .y_offset = 0
};

void find_red_cube_center(uint16 *scc8660_image, int *x, int *y) {
    int x_min = SCC8660_W, x_max = 0;
    int y_min = SCC8660_H, y_max = 0;

    uint32 pixel_count = 0;
    uint16 *p_pixel = scc8660_image;
    
    for (int cy = 0; cy < SCC8660_H; cy ++) {
        for (int cx = 0; cx < SCC8660_W; cx ++) {
            uint16 pixel = *p_pixel ++;
            
            // 提取 RGB 565 分量
            uint8 r = pixel >> 11;
            uint8 g = (pixel >> 5) & 0b111111;
            uint8 b = pixel & 0b11111;

            if (r > R_BASE_THRESHOLD && // R 分量大于阈值
                r > g &&                // R 分量显著大于 G（考虑位宽差异）
                r > (b << 1)            // R 分量显著大于 B
            ) {
                // 更新边界
                if (cx < x_min) x_min = cx;
                if (cx > x_max) x_max = cx;
                if (cy < y_min) y_min = cy;
                if (cy > y_max) y_max = cy;
                // 更新有效像素数RRR
                pixel_count ++;
            }
        }
    }

    // 有效性检查
    if (pixel_count > MIN_RED_PIXELS && 
        x_max > x_min && 
        y_max > y_min
    ) {
        *x = (x_min + x_max) >> 1;
        *y = (y_min + y_max) >> 1;
    }
    else {
        *x = -1;
        *y = -1;
    }
}
