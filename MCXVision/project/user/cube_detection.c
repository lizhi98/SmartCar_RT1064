#include "cube_detection.h"

// 立方体信息初始化

CubeInfo cube_info = {
    .exist = false,
    .pixel_count = 0,
    .x_center = 0,
    .y_center = 0,
    .x_min = 0,
    .x_max = 0,
    .y_min = 0,
    .y_max = 0
};

void parse_rgb(uint16 pixel, RGB *rgb) {
    pixel =  ((((uint16_t) pixel << 8) & 0xFF00) | ((uint16_t) pixel >> 8));
    rgb->r = (uint8) ((pixel & 0xf800) >> 8);
    rgb->g = (uint8) ((pixel & 0x07e0) >> 3);
    rgb->b = (uint8) ((pixel & 0x001f) << 3);
}

void find_red_cube_center(uint16 *scc8660_image) {
    int x_min = SCC8660_W, x_max = 0;
    int y_min = SCC8660_H, y_max = 0;

    uint32 pixel_count = 0;
    uint16 *p_pixel = scc8660_image;
    RGB rgb;

    for (int cy = 0; cy < SCC8660_H; cy ++) {
        for (int cx = 0; cx < SCC8660_W; cx ++) {
            uint16 pixel = *p_pixel;
            
            // 提取 RGB 565 分量
            parse_rgb(pixel, &rgb);

            if (rgb.r > R_BASE_THRESHOLD && // R 分量大于阈值
                (rgb.r >> 1) > rgb.g &&     // R 分量显著大于 G
                (rgb.r >> 1) > rgb.b        // R 分量显著大于 B
            ) {
                // 更新边界
                if (cx < x_min) x_min = cx;
                if (cx > x_max) x_max = cx;
                if (cy < y_min) y_min = cy;
                if (cy > y_max) y_max = cy;
                // 更新有效像素数
                pixel_count ++;
                // 将像素设置为黑色
                *p_pixel = RGB565_BLACK;
            }
            // 移动到下一个像素
            p_pixel ++;
        }
    }

    // 有效性检查
    if (pixel_count > MIN_RED_PIXELS && 
        x_max > x_min && 
        y_max > y_min
    ) {
        cube_info.exist = true;
        cube_info.x_center = (x_min + x_max) >> 1;
        cube_info.y_center = (y_min + y_max) >> 1;
        cube_info.x_min = x_min;
        cube_info.x_max = x_max;
        cube_info.y_min = y_min;
        cube_info.y_max = y_max;
        cube_info.pixel_count = pixel_count;
    }
    else {
        cube_info.exist = false;
    }
}
