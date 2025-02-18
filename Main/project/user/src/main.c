#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程

#include "main.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);  // 不可删除
    debug_init();                   // 调试端口初始化

    // 此处编写用户代码 例如外设初始化代码等
    
    // 此处编写用户代码 例如外设初始化代码等
    while(1)
    {
        // 此处编写需要循环执行的代码
        
        
        
        // 此处编写需要循环执行的代码
    }
}



