#include "zf_device_ips200.h"
#include "zf_device_scc8660.h"
#include "zf_driver_uart.h"

#include "rt1064_uart.h"
#include "cube_detection.h"

#include <stdbool.h>

char ips_buff[32];

#ifdef CUBE_DEBUG
uint16 scc8660_image_buff[SCC8660_W * SCC8660_H];
#else
#define scc8660_image_buff scc8660_image
#endif

int main(void)
{
    // 时钟和调试串口-串口4初始化
    zf_board_init();
    system_delay_ms(300);
    // 初始化 IPS200 模块
    ips200_init(); 
    // 等待 RT1064 模块唤醒
    rt1064_uart_init_wait();
    // 初始化 SCC8660 摄像头
    scc8660_init();

    while (1) {
        if (! scc8660_finish) continue;

        memcpy(scc8660_image_buff, scc8660_image, SCC8660_IMAGE_SIZE);

        find_red_cube_center(scc8660_image_buff);
        
        rt1064_uart_send_cube_info();

#ifdef CUBE_DEBUG
        ips200_show_scc8660(scc8660_image_buff);
        if (cube_info.exist) {
            int xc = cube_info.x_center;
            int yc = cube_info.y_center;
            int x1 = cube_info.x_min;
            int x2 = cube_info.x_max;
            int y1 = cube_info.y_min;
            int y2 = cube_info.y_max;
            // 绘制中心十字
            ips200_draw_line(xc - 5, yc, xc + 5, yc, RGB565_YELLOW);
            ips200_draw_line(xc, yc - 5, xc, yc + 5, RGB565_YELLOW);
            // 绘制边框
            ips200_draw_line(x1, y1, x2, y1, RGB565_GREEN);
            ips200_draw_line(x1, y1, x1, y2, RGB565_GREEN);
            ips200_draw_line(x2, y1, x2, y2, RGB565_GREEN);
            ips200_draw_line(x1, y2, x2, y2, RGB565_GREEN);
            // 显示数据
            sprintf(ips_buff, "pc: %5d xc: %3d yc: %3d", cube_info.pixel_count, cube_info.x_center, cube_info.y_center);
            ips200_show_string(0, 0, ips_buff);
        }
#endif

        scc8660_finish = 0;
    }
}
