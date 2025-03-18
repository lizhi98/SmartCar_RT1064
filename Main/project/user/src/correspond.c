#include "correspond.h"

// WIFI_UART数据接收缓冲区
static uint8 wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_TEMP_SIZE] = {0};
static char  host_cmd_recv_temp[HOST_CMD_SIZE_PLUS_ONE]                  = {0};
// HOST_CMD_RECV 超时计数器
// uint8 host_cmd_cycle_time = 0;
// // wifi_spi状态
// host_cmd_recv_state host_cmd_receive_flag = HOST_CMD_RECV_AVAI;
// // wifi_spi接收头数据缓存
// char host_cmd_recv_head = 0;
// // wifi_spi接收命令缓存
// char host_cmd_recv_cmd  = 0;
// // wifi_spi接收数据缓存
// char host_cmd_recv_data[14]   = {0};
// // wifi_spi接收尾数据缓存
// char host_cmd_recv_tail  = 0;

// ====================================WIFI SPI===============================================

// 函数简介     读取WIFI SPI的数据
// 参数说明     dst         要存入变量的地址
// 参数说明     dst_size_n_uint8    变量的字节数
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read(void * dst,uint32 dst_size_n_uint8){
    return (wifi_spi_read_buffer((uint8 *)dst,dst_size_n_uint8) != dst_size_n_uint8);
}

// =====================================WIFI UART==============================================

// 函数简介     读取WIFI UART接收到的的数据
// 参数说明     dst                 要存入变量的地址
// 参数说明     dst_size_n_uint8    要接收的字节数
// 返回参数     uint8               是否读取成功 0-成功 1-错误
uint8 wifi_uart_read_n_data(void * dst,uint32 dst_size_n_uint8){
    uint32 data_len = 0;
    // 从fifo缓冲区读取数据到数据缓冲区
    if( wifi_uart_read_buffer(&wifi_uart_read_buffer_temp[0],WIFI_UART_READ_BUFFER_TEMP_SIZE) != 0){
        // 判断是否为WIFI_UART的帧头
        if(memcmp(wifi_uart_read_buffer_temp,WIFI_UART_READ_BUFFER_HEAD,WIFI_UART_READ_BUFFER_HEAD_SIZE) == 0){
            // 从数据内容中获取数据长度
            data_len = wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_DATA_SIZE_POSI] - '0';
            // 读取数据到dst
            if(data_len >= dst_size_n_uint8){
                memcpy(dst,&wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_DATA_START],dst_size_n_uint8);
                // 清空数据缓冲区
                memset(wifi_uart_read_buffer_temp,0,WIFI_UART_READ_BUFFER_TEMP_SIZE);
                return 0;
            }
        }
    }
    // 清空数据缓冲区
    memset(wifi_uart_read_buffer_temp,0,WIFI_UART_READ_BUFFER_TEMP_SIZE);
    return 1;
}

// // 函数简介     读取WIFI UART data_lenth_n个位置的int32的数据(例如5个位置即读取"-1234"存成-1234)保存到dst指向的变量中
// // 参数说明     dst         要存入变量的地址
// // 返回参数     uint8       是否读取成功 0-成功 1-错误
// uint8 wifi_uart_read_int32(int32 * dst,int32 data_lenth_n){
//     // flag = 2;
//     int32 temp = 0;
//     if( !wifi_uart_read(host_cmd_recv_data,data_lenth_n) ){
//         host_cmd_recv_data[data_lenth_n] = '\0';
//         sscanf(host_cmd_recv_data,"%d",&temp);
//         *dst = temp;
//         // flag = 3;
//         return 0;
//     }
//     return 1;
// }

// // 函数简介     读取WIFI UART data_lenth_n个位置的float的数据(例如8个位置即读取"-1234.56"存成-1234.56)保存到dst指向的变量中
// // 参数说明     dst         要存入变量的地址
// // 返回参数     uint8       是否读取成功 0-成功 1-错误
// uint8 wifi_uart_read_float(float * dst,int32 data_lenth_n){
//     float temp = 0.0;
//     if( !wifi_uart_read(host_cmd_recv_data,data_lenth_n) ){
//         host_cmd_recv_data[data_lenth_n] = '\0';
//         sscanf(host_cmd_recv_data,"%f",&temp);
//         *dst = temp;
//         return 0;
//     }
//     return 1;
// }

// // 函数简介     读取WIFI UART data_lenth_n个位置的double的数据(例如8个位置即读取"-1234.56"存成-1234.56)保存到dst指向的变量中
// // 参数说明     dst         要存入变量的地址
// // 返回参数     uint8       是否读取成功 0-成功 1-错误
// uint8 wifi_uart_read_double(double * dst,int32 data_lenth_n){
//     double temp = 0.0;
//     if( !wifi_uart_read(host_cmd_recv_data,data_lenth_n) ){
//         host_cmd_recv_data[data_lenth_n] = '\0';
//         sscanf(host_cmd_recv_data,"%lf",&temp);
//         *dst = temp;
//         return 0;
//     }
//     return 1;
// }

// =====================================CORRESPOND_HOST_CMD(WIFI_UART)=========================

// // 上位机命令接收定时器中断回调函数
// void correspond_host_cmd_pit_call(void){
//     // 周期计数器自增
//     host_cmd_cycle_time++;
//     // flag =4;
//     switch (host_cmd_receive_flag)
//     {
//     // 空闲状态，扫描接收帧头
//     case HOST_CMD_RECV_AVAI:
//         // 空闲中超时正常，周期计数器清零
//         if(host_cmd_cycle_time > 10){
//             host_cmd_cycle_time = 0;
//         }
//         if( ! wifi_uart_read(&host_cmd_recv_head,1)){
//             if (host_cmd_recv_head == host_to_rt_head)
//             {   
//                 host_cmd_recv_head = 0;
//                 host_cmd_receive_flag = HOST_CMD_RECVD_HEAD;
//                 host_cmd_cycle_time = 0;
//             }
//         }
//         // wifi_spi_send_string("avai\n");
//         return;
//     // 接收到帧头，扫描接收命令
//     case HOST_CMD_RECVD_HEAD:
//         // 长时间未接收到命令，返回空闲状态
//         if(host_cmd_cycle_time > 20){
//             host_cmd_cycle_time = 0;
//             host_cmd_receive_flag = HOST_CMD_RECV_AVAI;
//         }
//         if( ! wifi_uart_read(&host_cmd_recv_cmd,1)){
//             host_cmd_receive_flag = HOST_CMD_RECVD_CMD;
//             host_cmd_cycle_time = 0;
//         }
//         // wifi_spi_send_string("head\n");
//         break;
//     case HOST_CMD_RECVD_CMD:
//         // flag = 5;
//         // 长时间未接收到数据，返回空闲状态
//         if(host_cmd_cycle_time > 20){
//             host_cmd_cycle_time = 0;
//             host_cmd_receive_flag = HOST_CMD_RECV_AVAI;
//         }
//         switch (host_cmd_recv_cmd)
//         {
//         // a --> target_speed_magnitude
//         case 'a':
//             // flag = 6;
//             if( !wifi_uart_read_int32(&target_speed_magnitude,HOST_CMD_INT32_LENGTH) ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'b':
//             if(  !wifi_uart_read_float(&(motors[LEFT].PID->KP),HOST_CMD_FLOAT_LENGTH) ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'c':
//             if(  !wifi_uart_read_float(&(motors[LEFT].PID->KI),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'd':
//             if(  !wifi_uart_read_float(&(motors[LEFT].PID->KD),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         // RIGHT
//         case 'e':
//             if(  !wifi_uart_read_float(&(motors[RIGHT].PID->KP),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'f':
//             if(  !wifi_uart_read_float(&(motors[RIGHT].PID->KI),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'g':
//             if(  !wifi_uart_read_float(&(motors[RIGHT].PID->KD),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         // REAR
//         case 'h':
//             if(  !wifi_uart_read_float(&(motors[REAR].PID->KP),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'i':
//             if(  !wifi_uart_read_float(&(motors[REAR].PID->KI),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         case 'j':
//             if(  !wifi_uart_read_float(&(motors[REAR].PID->KD),HOST_CMD_FLOAT_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         // target_angle
//         case 'k':
//             if(  !wifi_uart_read_double(&(target_angle),HOST_CMD_DOUBLE_LENGTH)  ){
//                 host_cmd_receive_flag = HOST_CMD_RECVD_DATA;
//                 host_cmd_cycle_time = 0;
//             }
//             break;
//         default:
//             // flag = 6;
//             break;
//         }
//         // 清空命令缓冲区
//         host_cmd_recv_cmd = 0;
//         break;
//     case HOST_CMD_RECVD_DATA:
//         // 长时间未接收到帧尾，返回空闲状态
//         if(host_cmd_cycle_time > 20){
//             host_cmd_cycle_time = 0;
//             host_cmd_receive_flag = HOST_CMD_RECV_AVAI;
//         }
//         if( ! wifi_uart_read(&host_cmd_recv_tail,1)){
//             if (host_cmd_recv_tail == host_to_rt_tail)
//             {
//                 host_cmd_receive_flag = HOST_CMD_RECV_AVAI;
//                 host_cmd_cycle_time = 0;
//             }
//         }
//         // wifi_spi_send_string("data\n");
//         break;
//     default:
//         host_cmd_cycle_time = 0;
//         break;
//     }
// }

void correspond_host_cmd_pit_call(void){
    if(! wifi_uart_read_n_data(host_cmd_recv_temp,HOST_CMD_SIZE_PLUS_ONE - 1)){
        switch (host_cmd_recv_temp[0])
        {
        // target_motion
        case 'a':
            sscanf(&host_cmd_recv_temp[2],"%d",&target_speed_magnitude);
            break;
        case 'b':
            sscanf(&host_cmd_recv_temp[2],"%f",&target_angle);
            break;
        // PID
        case 'c':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KP));
            break;
        case 'd':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KI));
            break;
        case 'e':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KD));
            break;
        case 'f':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KP));
            break;
        case 'g':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KI));
            break;
        case 'h':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KD));
            break;
        case 'i':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KP));
            break;
        case 'j':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KI));
            break;
        case 'k':
            sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KD));
            break;
        default:
            break;
        }
    }
    // 清空命令缓存
    memset(host_cmd_recv_temp,0,HOST_CMD_SIZE_PLUS_ONE);
}

void correspond_host_cmd_pit_init(void){
    pit_ms_init(HOST_CMD_RECV_PIT,HOST_CMD_RECV_PIT_MS);
}

uint8 correspond_host_cmd_init(void){
    return (wifi_uart_init(WIFI_SSID,WIFI_PASS,WIFI_UART_STATION) || 
            wifi_uart_connect_tcp_servers(HOST_IP,WIFI_UART_HOST_PORT,WIFI_UART_COMMAND));
}

// ================CORRESPOND_IMAGE_SEND(WIFI_SPI)====================

// 函数简介     图传（WIFI_SPI）初始化
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
uint8 correspond_image_send_init(void){
    if( ! (wifi_spi_init(WIFI_SSID,WIFI_PASS) || 
           wifi_spi_socket_connect(WIFI_SPI_CONNECT_MODE,HOST_IP,WIFI_SPI_HOST_PORT,WIFI_SPI_LOCAL_PORT)))
    {

        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X,mt9v03x_image[0],MT9V03X_W,MT9V03X_H);
        return 0;
    }
    return 1;
}

void correspond_image_send_call(void){
    seekfree_assistant_camera_send();
}

// ====================UART TO OpenART mini====================
// void uart_init_(void){
//     uart_init(UART_N,UART_BAUD,UART_TX_PIN,UART_RX_PIN);
// }


