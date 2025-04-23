#if defined(__cplusplus) && ! defined(_VSCODE)
extern "C"
{
#endif /* __cplusplus */ 

#include "zf_device_ips200.h"
#include "zf_device_scc8660.h"
#include "zf_driver_uart.h"

#include "RT1064.h"
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

    while (1) {
        // 立方体状态为不在视野内
        cube_info.state = CUBE_OUTSIDE_VIEW;
        if (scc8660_finish) {
            int center_x, center_y;
            find_red_cube_center(scc8660_image, &center_x, &center_y);
            
            if (center_x != -1) {
                cube_info.x_offset = center_x;
                cube_info.y_offset = center_y;
                ips200_draw_line(center_x - 5, center_y, center_x + 5, center_y, RGB565_RED);
                ips200_draw_line(center_x, center_y - 5, center_x, center_y + 5, RGB565_RED);
            }
            
            rt1064_uart_send_cube_info();
            ips200_show_scc8660(scc8660_image);
            scc8660_finish = 0;
        }
    }
}

#if defined(__cplusplus) && ! defined(_VSCODE)
}
#endif