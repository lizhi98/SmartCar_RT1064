#if defined(__cplusplus) && !defined(_VSCODE)
extern "C" // mian文件是C++文件，如果需要包含C语言的头文件，就需要使用extern "C"
{
#endif /* __cplusplus */ 

#include "zf_device_ips200.h"
#include "zf_device_scc8660.h"
#include "zf_driver_uart.h"

#include "RT1064.h"
#include "cube_detection.h"

// 核心板下载完代码需要手动复位！！！
// 如果程序运行后，模块上的Core灯闪烁，说明程序进入了HardFault，就需要检测是否有数组越界，外设没有初始化，外设时钟没有设置等情况

int main(void)
{
    // 时钟和调试串口-串口4初始化
    zf_board_init();
    // 延时300ms
    system_delay_ms(300);
    // 使用C++编译无法使用printf，可以使用zf_debug_printf和zf_user_printf替代
    // zf_debug_printf("debug_uart_init_finish\r\n");  // 使用调试串口-串口4发送数据
    // zf_user_printf("user_uart_init_finish\r\n");    // 使用用户串口-串口5发送数据

    // ================外设初始化================
    
    ips200_init(); // 初始化IPS200模块
    rt1064_uart_init_wait(); // 等待RT1064模块唤醒
    scc8660_init();
    // ================外设初始化================

    while (1)
    {   
        if(scc8660_finish){
            ips200_show_scc8660(scc8660_image);
            scc8660_finish = 0;
        }

    }
}

#if defined(__cplusplus)&&!defined(_VSCODE)
}
#endif