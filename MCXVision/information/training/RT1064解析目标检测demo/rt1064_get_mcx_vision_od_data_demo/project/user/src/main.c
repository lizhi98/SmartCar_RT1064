/*********************************************************************************************************************
* RT1064DVL6A Opensourec Library ����RT1064DVL6A ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
* 
* ���ļ��� RT1064DVL6A ��Դ���һ����
* 
* RT1064DVL6A ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
* 
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
* 
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
* 
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
* 
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 8.32.4 or MDK 5.33
* ����ƽ̨          RT1064DVL6A
* ��������          https://seekfree.taobao.com/
* 
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-21        SeekFree            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// *************************** ����Ӳ������˵�� ***************************
// ʹ����ɿƼ� CMSIS-DAP | ARM ��������������
//      ֱ�ӽ���������ȷ�����ں��İ�ĵ������ؽӿڼ���
// 
// ʹ�� MCX_Vision ģ������ �Ƽ�ʹ��1064ѧϰ��
//      ģ��ܽ�            ��Ƭ���ܽ�
//      MCX_Vision_RX         C17
//      MCX_Vision_TX         C16
//      MCX_Vision_GND        GND
//      MCX_Vision_5V         ��Դ
//      ģ��ܽ�            ��Ƭ���ܽ�

//      ˫������ ���������� Ӳ������
//      RD                  �鿴 zf_device_ips200.h �� IPS200_RD_PIN_PARALLEL8     �궨�� B0 
//      WR                  �鿴 zf_device_ips200.h �� IPS200_WR_PIN_PARALLEL8     �궨�� B1 
//      RS                  �鿴 zf_device_ips200.h �� IPS200_RS_PIN_PARALLEL8     �궨�� B2 
//      RST                 �鿴 zf_device_ips200.h �� IPS200_RST_PIN_PARALLEL8    �궨�� C19
//      CS                  �鿴 zf_device_ips200.h �� IPS200_CS_PIN_PARALLEL8     �궨�� B3 
//      BL                  �鿴 zf_device_ips200.h �� IPS200_BL_PIN_PARALLEL8     �궨�� C18
//      D0-D7               �鿴 zf_device_ips200.h �� IPS200_Dx_PIN_PARALLEL8     �궨�� B16/B17/B18/B19/D12/D13/D14/D15
//      GND                 ���İ��Դ�� GND
//      3V3                 ���İ� 3V3 ��Դ
//      �������� SPI ������ Ӳ������
//      SCL                 �鿴 zf_device_ips200.h �� IPS200_SCL_PIN_SPI  �궨��  B0
//      SDA                 �鿴 zf_device_ips200.h �� IPS200_SDA_PIN_SPI  �궨��  B1
//      RST                 �鿴 zf_device_ips200.h �� IPS200_RST_PIN_SPI  �궨��  B2
//      DC                  �鿴 zf_device_ips200.h �� IPS200_DC_PIN_SPI   �궨��  C19
//      CS                  �鿴 zf_device_ips200.h �� IPS200_CS_PIN_SPI   �궨��  B3 
//      BL                  �鿴 zf_device_ips200.h �� IPS200_BLk_PIN_SPI  �궨��  C18
//      GND                 ���İ��Դ�� GND
//      3V3                 ���İ� 3V3 ��Դ
// *************************** ���̲���˵�� ***************************
// 1.MCX Vision���غ�Ŀ���������
// 
// 2.ʹ��MCX Vision���ӵ��������İ�Ĵ���4������IPS2.0����Ļ���ڶϵ�������������
// 
// 3.���İ���¼��ɱ�����
// 
// 4.��λ�󣬿�������Ļ�Ͽ���Ŀ���ⷵ�ص���Ϣ�����û����Ļ�����Խ�����Բ鿴od_result����

// **************************** �������� ****************************
#define UART_INDEX              (UART_4   )                             
#define UART_BAUDRATE           (115200)                                
#define UART_TX_PIN             (UART4_TX_C16  )                        
#define UART_RX_PIN             (UART4_RX_C17  )                        
#define UART_PRIORITY           (LPUART4_IRQn)                                  

#define IPS200_TYPE             (IPS200_TYPE_SPI)   // ˫������ ���������� ����궨����д IPS200_TYPE_PARALLEL8
                                                    // �������� SPI ������ ����궨����д IPS200_TYPE_SPI

uint8 uart_get_data[64];                            // ���ڽ������ݻ�����
uint8 fifo_get_data[64];                            // fifo �������������
                    
uint32 fifo_data_count = 0;                         // fifo ���ݸ���
fifo_struct uart_data_fifo;

typedef struct
{
    uint16 res_x1;
    uint16 res_y1;
    uint16 res_x2;
    uint16 res_y2;
}od_result_t;
volatile od_result_t od_result[10];

int main(void)
{
    clock_init(SYSTEM_CLOCK_600M);                  // ����ɾ��
    debug_init();                                   // ���Զ˿ڳ�ʼ��
    
    system_delay_ms(300);
    // �˴���д�û����� ���������ʼ�������
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // ��ʼ�� fifo ���ػ�����

    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // ��ʼ������
    uart_rx_interrupt(UART_INDEX, ZF_ENABLE);                                   // ���� UART_INDEX �Ľ����ж�
    interrupt_set_priority(UART_PRIORITY, 0);                                   // ���ö�Ӧ UART_INDEX ���ж����ȼ�Ϊ 0

    ips200_init(IPS200_TYPE);                                                   // ��ʼ����Ļ
    
    // ��ʾ�̶��ı����
    for(uint8 i = 0; i < 8; i++)
    {
        ips200_show_string(0, i * 40, "od_res_ :  x1   y1   x2   y2");
        ips200_show_uint(56, i * 40, i, 1);
        ips200_draw_line(0, 40 * (i + 1) - 2, 239, 40 * (i + 1) - 2, RGB565_BLACK);
    }
        
    while(1)
    {
        // ��ʾĿ������
        for(uint8 i = 0; i < 8; i++)
        {
            ips200_show_uint(85, i * 40 + 20, od_result[i].res_x1, 3);
            ips200_show_uint(125, i * 40 + 20, od_result[i].res_y1, 3);
            ips200_show_uint(165, i * 40 + 20, od_result[i].res_x2, 3);
            ips200_show_uint(205, i * 40 + 20, od_result[i].res_y2, 3);
        }
        system_delay_ms(10);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     UART_INDEX �Ľ����жϴ����� ����������� UART_INDEX ��Ӧ���жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{ 
    uint8 get_data = 0;                                                             // �������ݱ���
    uint32 temp_length = 0;
    uint8 od_num = 0;
    uart_query_byte(UART_INDEX, &get_data);  
    {
        fifo_write_buffer(&uart_data_fifo, &get_data, 1);   
    }
    
    if(0xFF == get_data)
    {
        // ��ȡ��1�����ݣ������ж�֡ͷ��ʹ�������������
        temp_length = 1;
        fifo_read_buffer(&uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
        if(0xAA == fifo_get_data[0])
        {
            // ��ȡ��1�����ݣ����ڻ�ȡĿ����ţ�ʹ�������������
            temp_length = 1;
            fifo_read_buffer(&uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
            od_num = fifo_get_data[0];
            // ��ȡ8�����ݣ����ڻ�ȡĿ�����ݣ�Ȼ��ת�Ƶ��ṹ��������
            temp_length = 8;
            fifo_read_buffer(&uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
            memcpy((uint8*)(&od_result[od_num]), fifo_get_data, 8);
        }
        fifo_clear(&uart_data_fifo);
    }
}
// **************************** �������� ****************************
