#include "correspond.h"

// WIFI_UART数据接收缓冲区
uint8 wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_TEMP_SIZE] = {0};
char  host_cmd_recv_temp[HOST_CMD_SIZE_PLUS_ONE]                  = {0};

// WIFI_UART数据发送缓冲区
char  wifi_uart_send_buffer_temp[WIFI_UART_SEND_BUFFER_TEMP_SIZE] = {0};

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
        if(memcmp(&wifi_uart_read_buffer_temp[0],WIFI_UART_READ_BUFFER_HEAD,WIFI_UART_READ_BUFFER_HEAD_SIZE) == 0){
            // 读取数据到dst
            memcpy(dst,&wifi_uart_read_buffer_temp[WIFI_UART_READ_BUFFER_HEAD_SIZE],dst_size_n_uint8);
            // 清空数据缓冲区
            memset(wifi_uart_read_buffer_temp,0,WIFI_UART_READ_BUFFER_TEMP_SIZE);
            
            return 0;
        }
    }
    // 清空数据缓冲区
    memset(wifi_uart_read_buffer_temp,0,WIFI_UART_READ_BUFFER_TEMP_SIZE);
    return 1;
}


void correspond_host_cmd_pit_call(void){
    return;
    // if(! wifi_uart_read_n_data(host_cmd_recv_temp,HOST_CMD_SIZE_PLUS_ONE - 1)){
    //     switch (host_cmd_recv_temp[0])
    //     {
    //     case 's':
    //         // flag = 1;
    //         target_speed_magnitude = 0;
    //         break;
    //     case 'a':
    //         // flag = 1;
    //         sscanf(&host_cmd_recv_temp[2],"%d",&target_speed_magnitude);
    //         break;
    //     case 'b':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&rotation_pid.KP);
    //         break;
    //     // PID
    //     case 'c':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KP));
    //         break;
    //     case 'd':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KI));
    //         break;
    //     case 'e':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[LEFT].PID->KD));
    //         break;
    //     case 'f':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KP));
    //         break;
    //     case 'g':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KI));
    //         break;
    //     case 'h':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[RIGHT].PID->KD));
    //         break;
    //     case 'i':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KP));
    //         break;
    //     case 'j':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KI));
    //         break;
    //     case 'k':
    //         sscanf(&host_cmd_recv_temp[2],"%f",&(motors[REAR].PID->KD));
    //         break;
    //     default:
    //         break;
    //     }
    // }
    // // 清空命令缓存
    // memset(host_cmd_recv_temp,0,HOST_CMD_SIZE_PLUS_ONE);
    

}

// 函数名称     correspond_send_info_to_host
// 函数简介     将信息发送到上位机
void correspond_send_info_to_host(){
#if CORRESPOND_SEND_INFO_MODE == 1
    // sprintf(wifi_uart_send_buffer_temp, 
    //     "%d.0,%d.0,%d.0,%f,%d,%d,%d,%d,%d\n",
    //     motors[LEFT].current_speed, motors[RIGHT].current_speed, motors[REAR].current_speed,  // 左右后电机速度
    //     image_result.offset,  image_process_time,// 图像
    //     motors[LEFT].pwm_duty,motors[RIGHT].pwm_duty,motors[REAR].pwm_duty,  // 电机实时PWM
    //     rotation_pid.wl_out  // 角速度PID输出
    // );
    sprintf(wifi_uart_send_buffer_temp, 
        "%lf,%d,%d,%d\n",
        image_result.offset,  image_process_time,image_result.element_type,
        rotation_pid.wl_out  // 角速度PID输出
    );
#elif CORRESPOND_SEND_INFO_MODE == 2
    sprintf(wifi_uart_send_buffer_temp, 
        "%f,%f,%f\n",
        ICM42688.gyro_z, gyroscope_result.angle_z,gyro_z_offset  // 陀螺仪数据
    );
#endif
    wifi_uart_send_buffer((uint8 *)wifi_uart_send_buffer_temp, strlen(wifi_uart_send_buffer_temp));
}

void correspond_host_cmd_pit_init(void){
    pit_ms_init(HOST_CMD_RECV_PIT,HOST_CMD_RECV_PIT_MS);
}

uint8 correspond_host_cmd_init(void){
    return (wifi_uart_init(WIFI_SSID,WIFI_PASS,WIFI_UART_STATION) || 
            wifi_uart_connect_udp_client(HOST_IP,WIFI_UART_HOST_PORT,WIFI_UART_LOCAL_PORT,WIFI_UART_SERIANET));
}

// ================CORRESPOND_IMAGE_SEND(WIFI_SPI)====================

// 函数简介     图传（WIFI_SPI）初始化
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
uint8 correspond_image_send_init(void){
    if( ! (wifi_spi_init(WIFI_SSID,WIFI_PASS) || 
           wifi_spi_socket_connect(WIFI_SPI_CONNECT_MODE,HOST_IP,WIFI_SPI_HOST_PORT,WIFI_SPI_LOCAL_PORT)))
    {

        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
        seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
        return 0;
    }
    return 1;
}

void correspond_image_send_call(void){
    seekfree_assistant_camera_send();
}

