#include "correspond.h"
#include "string.h"
#include "math.h"
// 超时计数器
uint8 wifi_spi_cycle_time = 0;
// wifi_spi状态
wifi_spi_state wifi_spi_receive_flag = WIFI_SPI_AVAI;
// wifi_spi接收头数据缓存
char wifi_spi_recv_head = 0;
// wifi_spi接收命令缓存
char wifi_spi_recv_cmd  = 0;
// wifi_spi接收数据缓存
char wifi_spi_recv_float_data[9]   = {0};
char wifi_spi_recv_int_data[6]     = {0};
// wifi_spi接收尾数据缓存
char wifi_spi_recv_tail  = 0;

// ================WIFI SPI====================

// 函数简介     WiFi 模块初始化
// 参数说明     uint16 time_out_s  超时时间 单位s 尝试周期5s
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
uint8 wifi_spi_init_(){
    return wifi_spi_init(WIFI_SSID,WIFI_PASS);
}

// 函数简介     读取WIFI SPI的数据
// 参数说明     dst         要存入变量的地址
// 参数说明     dst_size_n_uint8    变量的字节数
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read(void * dst,uint32 dst_size_n_uint8){
    return (wifi_spi_read_buffer((uint8 *)dst,dst_size_n_uint8) != dst_size_n_uint8);
}

// 函数简介     读取WIFI SPI 5个位置的int的数据(例如读取"-1234"存成-1234)
// 参数说明     dst         要存入变量的地址
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read_5int(int32 * dst){
    int32 temp = 0;
    if( !wifi_spi_read(wifi_spi_recv_int_data,5) ){
        wifi_spi_recv_int_data[5] = '\0';
        sscanf(wifi_spi_recv_int_data,"%d",&temp);
        *dst = temp;
        return 0;
    }
    return 1;
}

// 函数简介     读取WIFI SPI 8个位置的float的数据(例如读取"-1234.56"存成-1234.56)
// 参数说明     dst         要存入变量的地址
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read_8float(float * dst){
    float temp = 0.0;
    if( !wifi_spi_read(wifi_spi_recv_float_data,8) ){
        wifi_spi_recv_float_data[8] = '\0';
        sscanf(wifi_spi_recv_float_data,"%f",&temp);
        *dst = temp;
        return 0;
    }
    return 1;
}

// 函数简介     读取WIFI SPI 8个位置的double的数据(例如读取"-1234.56"存成-1234.56)
// 参数说明     dst         要存入变量的地址
// 返回参数     uint8       是否读取成功 0-成功 1-错误
uint8 wifi_spi_read_8double(double * dst){
    double temp = 0.0;
    if( !wifi_spi_read(wifi_spi_recv_float_data,8) ){
        wifi_spi_recv_float_data[8] = '\0';
        sscanf(wifi_spi_recv_float_data,"%lf",&temp);
        *dst = temp;
        return 0;
    }
    return 1;
}

// wifi_spi中断回调函数
void wifi_spi_pit_call(void){
    // 周期计数器自增
    wifi_spi_cycle_time++;
    
    switch (wifi_spi_receive_flag)
    {
    // 空闲状态，扫描接收帧头
    case WIFI_SPI_AVAI:
        // 空闲中超时正常，周期计数器清零
        if(wifi_spi_cycle_time > 10){
            wifi_spi_cycle_time = 0;
        }
        if( ! wifi_spi_read(&wifi_spi_recv_head,1)){
            if (wifi_spi_recv_head == host_to_rt_head)
            {
                wifi_spi_receive_flag = WIFI_SPI_RECVD_HEAD;
                wifi_spi_cycle_time = 0;
            }
        }
        // wifi_spi_send_string("avai\n");
        return;
    // 接收到帧头，扫描接收命令
    case WIFI_SPI_RECVD_HEAD:
        // 长时间未接收到命令，返回空闲状态
        if(wifi_spi_cycle_time > 9){
            wifi_spi_cycle_time = 0;
            wifi_spi_receive_flag = WIFI_SPI_AVAI;
        }
        if( ! wifi_spi_read(&wifi_spi_recv_cmd,1)){
            wifi_spi_receive_flag = WIFI_SPI_RECVD_CMD;
            wifi_spi_cycle_time = 0;
        }
        // wifi_spi_send_string("head\n");
        break;
    case WIFI_SPI_RECVD_CMD:
        // 长时间未接收到数据，返回空闲状态
        if(wifi_spi_cycle_time > 8){
            wifi_spi_cycle_time = 0;
            wifi_spi_receive_flag = WIFI_SPI_AVAI;
        }
        switch (wifi_spi_recv_cmd)
        {
        // a --> target_speed_magnitude
        case 'a':
            if(!wifi_spi_read_5int(&target_speed_magnitude)){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'b':
            if(!wifi_spi_read_8float(&(motors[LEFT].PID->KP))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'c':
            if(!wifi_spi_read_8float(&(motors[LEFT].PID->KI))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'd':
            if(!wifi_spi_read_8float(&(motors[LEFT].PID->KD))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        // RIGHT
        case 'e':
            if(!wifi_spi_read_8float(&(motors[RIGHT].PID->KP))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'f':
            if(!wifi_spi_read_8float(&(motors[RIGHT].PID->KI))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'g':
            if(!wifi_spi_read_8float(&(motors[RIGHT].PID->KD))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        // REAR
        case 'h':
            if(!wifi_spi_read_8float(&(motors[REAR].PID->KP))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'i':
            if(!wifi_spi_read_8float(&(motors[REAR].PID->KI))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        case 'j':
            if(!wifi_spi_read_8float(&(motors[REAR].PID->KD))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        // target_angle
        case 'k':
            if(!wifi_spi_read_8double(&(target_angle))){
                wifi_spi_receive_flag = WIFI_SPI_RECVD_DATA;
                wifi_spi_cycle_time = 0;
            }
            break;
        default:
            break;
        }

        break;
    case WIFI_SPI_RECVD_DATA:
        // 长时间未接收到帧尾，返回空闲状态
        if(wifi_spi_cycle_time > 7){
            wifi_spi_cycle_time = 0;
            wifi_spi_receive_flag = WIFI_SPI_AVAI;
        }
        if( ! wifi_spi_read(&wifi_spi_recv_tail,1)){
            if (wifi_spi_recv_tail == host_to_rt_tail)
            {
                wifi_spi_receive_flag = WIFI_SPI_AVAI;
                wifi_spi_cycle_time = 0;
            }
        }
        // wifi_spi_send_string("data\n");
        break;
    default:
        wifi_spi_cycle_time = 0;
        break;
    }
}

void wifi_spi_pit_init(void){
    pit_ms_init(WIFI_SPI_RECV_PIT,WIFI_SPI_RECV_PIT_MS);
}

// ====================UART TO OpenART mini====================
void uart_init_(void){
    uart_init(UART_N,UART_BAUD,UART_TX_PIN,UART_RX_PIN);
}


