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

uint8 pixel_x_count[SCC8660_W];
uint8 pixel_y_count[SCC8660_H];

void find_red_cube_center(uint16 *scc8660_image) {
    int16 x_min = 0, x_max = SCC8660_W - 1;
    int16 y_min = 0, y_max = SCC8660_H - 1;

    memset(pixel_x_count, 0, sizeof(pixel_x_count));
    memset(pixel_y_count, 0, sizeof(pixel_y_count));

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
                // 更新有效像素数
                pixel_count ++;
                pixel_x_count[cx] ++;
                pixel_y_count[cy] ++;

#ifdef CUBE_DEBUG
                // 将像素设置为黑色
                *p_pixel = RGB565_BLACK;
#endif
            }
            // 移动到下一个像素
            p_pixel ++;
        }
    }

    // 有效性检查
    if (pixel_count < MIN_PIXEL_COUNT) {
        cube_info.exist = false;
        return;
    }

    // 计算边界
    while (pixel_x_count[x_min] < MIN_X_PIXEL_COUNT && x_min < SCC8660_W) x_min ++;
    while (pixel_x_count[x_max] < MIN_X_PIXEL_COUNT && x_max >= 0) x_max --;
    while (pixel_y_count[y_min] < MIN_Y_PIXEL_COUNT && y_min < SCC8660_H) y_min ++;
    while (pixel_y_count[y_max] < MIN_Y_PIXEL_COUNT && y_max >= 0) y_max --;
    if (x_min == SCC8660_W || y_min == SCC8660_H || x_max == - 1 || y_max == - 1) {
        cube_info.exist = false;
        return;
    }

    // 写入立方体信息
    cube_info.exist = true;
    cube_info.x_center = (x_min + x_max) >> 1;
    cube_info.y_center = (y_min + y_max) >> 1;
    cube_info.x_min = x_min;
    cube_info.x_max = x_max;
    cube_info.y_min = y_min;
    cube_info.y_max = y_max;
    cube_info.pixel_count = pixel_count;
}
