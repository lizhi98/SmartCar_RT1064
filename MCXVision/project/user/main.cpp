#if defined(__cplusplus) && ! defined(_VSCODE)
extern "C"
{
#endif /* __cplusplus */ 

#include "zf_device_ips200.h"
#include "zf_device_scc8660.h"
#include "zf_driver_uart.h"

#include "rt1064_uart.h"
#include "cube_detection.h"

#include <stdbool.h>

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

    char ips_buff[32];

    while (1) {
        if (! scc8660_finish) continue;

        find_red_cube_center(scc8660_image);
        
        // rt1064_uart_send_cube_info();

        ips200_show_scc8660(scc8660_image);
        if (cube_debug_info.exist) {
            int xc = cube_debug_info.x_center;
            int yc = cube_debug_info.y_center;
            int x1 = cube_debug_info.x_min;
            int x2 = cube_debug_info.x_max;
            int y1 = cube_debug_info.y_min;
            int y2 = cube_debug_info.y_max;
            // 绘制中心十字
            ips200_draw_line(xc - 5, yc, xc + 5, yc, RGB565_YELLOW);
            ips200_draw_line(xc, yc - 5, xc, yc + 5, RGB565_YELLOW);
            // 绘制边框
            ips200_draw_line(x1, y1, x2, y1, RGB565_GREEN);
            ips200_draw_line(x1, y1, x1, y2, RGB565_GREEN);
            ips200_draw_line(x2, y1, x2, y2, RGB565_GREEN);
            ips200_draw_line(x1, y2, x2, y2, RGB565_GREEN);
            // 显示数据
            sprintf(ips_buff, "pc: %5d", cube_debug_info.pixel_count);
            ips200_show_string(0, 0, ips_buff);
        }

        scc8660_finish = 0;
    }
}

#if defined(__cplusplus) && ! defined(_VSCODE)
}
#endif