// #include "zf_common_headfile.h"

// // 打开新的工程或者工程移动了位置务必执行以下操作
// // 第一步 关闭上面所有打开的文件
// // 第二步 project->clean  等待下方进度条走完

// // 本例程是开源库移植用空工程
// #include "main.h"


// int main(void)
// {
//     clock_init(SYSTEM_CLOCK_600M);  // 不可删除
//     debug_init();                   // 调试端口初始化
//     // =====================外设初始化======================
//     // 硬件初始化正常标志位         1-初始化失败 0-初始化成功
//     uint8 hardware_init_flag = 0;
//     // 屏幕初始化
//     screen_init();
//     // WIFI SPI 初始化
//     // hardware_init_flag = wifi_spi_init_(20);
//     hardware_init_flag = wifi_spi_init_();
//     hardware_init_flag = wifi_spi_socket_connect("TCP","192.168.137.1","9894","6060");
//     // 摄像头初始化，逐飞助手图像信息初始化
//     hardware_init_flag = mt_camera_init();
//     seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X,mt9v03x_image[0],MT9V03X_W,MT9V03X_H);
//     // 初始化完成，屏幕输出初始化结果
//     if(hardware_init_flag){screen_show_str(0,0,"HW Init Fail!!!"); while(1){};} // 程序停止
//     else                  {screen_show_str(0,0,"HW Init Success");}
//     system_delay_ms(2000);
//     screen_clear();
//     // =====================外设初始化======================
//     seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
//     while(1)
//     {
//         if(mt9v03x_finish_flag){
//             seekfree_assistant_camera_send();
//             mt9v03x_finish_flag = 0;
//         }
//     }
// }



