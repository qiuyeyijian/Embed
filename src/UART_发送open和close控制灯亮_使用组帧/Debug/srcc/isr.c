//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"

//�ڲ���������
uint8_t CreateFrame(uint8_t Data, uint8_t* buffer);
uint8_t is_equal(uint8_t* str);

//======================================================================
//�жϷ���������ƣ�UART_USER_Handler
//����������UART_USE�����յ�һ���ֽڴ���
//��    �ܣ��յ�һ���ֽڣ�ֱ�ӷ��ظ��ֽ�
//��    ע�����뱾����󣬿�ʹ��uart_get_re_int�������ٽ����жϱ�־�ж�
//          ��1-��UART�����жϣ�0-û��UART�����жϣ�
//======================================================================
void UART_User_Handler(void)
{

    uint8_t ch, flag;
    DISABLE_INTERRUPTS; //�����ж�
    //-----------------------------------------------------------------------
    ch = uart_re1(UART_User, &flag); //���ý���һ���ֽڵĺ�����������ж�λ
    //�����ڲ�����CreateFrame������֡
    // uart_send1(UART_User,ch);//�ط����յ����ֽ�
    if (CreateFrame(ch, g_uart_recvBuf) != 0) //��֡�ɹ�
    {
        uart_send_string(UART_User, " Please send close or open!\n");
        // uint8_t open_str[] = "open";
        // uint8_t close_str[] = "close";

        // // �ж��Ƿ���open

        // if (is_equal(open_str))
        // {
        //     uart_send_string(UART_User, " Success!\n");
        //     gpio_set(LIGHT_BLUE, LIGHT_ON);
        // }
        // else if (is_equal(close_str))
        // {
        //     uart_send_string(UART_User, " Success!\n");
        //     gpio_set(LIGHT_BLUE, LIGHT_OFF);
        // }
        // else
        // {
        //     uart_send_string(UART_User, " Please send close or open!\n");
        // }
    }

    ENABLE_INTERRUPTS; //�����ж�
}

//�ڲ����ú���

//===========================================================================
//�������ƣ�CreateFrame
//���ܸ�Ҫ���齨����֡��������֡���ݼ��뵽����֡��
//����˵����Data��                   ����֡����
//          buffer:        ����֡����
//�������أ���֡״̬    0-��֡δ�ɹ���1-��֡�ɹ�
//��ע��ʮ����������֡��ʽ
//               ֡ͷ        + ���ݳ���         + ��Ч����    +  ֡β
//           FrameHead   +   len       + ��Ч����    + FrameTail
//===========================================================================

#define FrameHead (0x50) //֡ͷ     ASCII���ӦP
#define FrameTail (0x43) //֡β     ASCII���ӦC
//P4openC
uint8_t CreateFrame(uint8_t Data, uint8_t* buffer)
{
    static uint8_t frameLen = 0; //֡�ļ�����
    uint8_t frameFlag;           //��֡״̬

    frameFlag = 0; //��֡״̬��ʼ��
    //���ݾ�̬����frameCount��֡
    switch (frameLen)
    {
    case 0: //��һ������
    {
        if (Data == FrameHead) //�յ�������֡ͷFrameHead
        {
            buffer[0] = Data;
            frameLen++;
            frameFlag = 0; //��֡��ʼ
        }
        break;
    }
    case 1: //�ڶ������ݣ��������������յ����ݸ���
    {
        buffer[1] = Data - 0x30;
        frameLen++;
        break;
    }
    default: //�������
    {
        //�ڶ�λ��������Ч���ݳ���,�������������µ�����ֱ��֡βǰһλ
        if (frameLen >= 2 && frameLen <= (buffer[1] + 1))
        {
            buffer[frameLen] = Data;
            frameLen++;
            break;
        }

        //����ĩβ������ִ��
        if (frameLen >= (buffer[1] + 2))
        {
            if (Data == FrameTail) //����֡β
            {
                buffer[frameLen] = Data; //��֡β���뻺����
                frameFlag = 1;           //��֡�ɹ�
            }
            frameLen = 0; //������0��׼��������֡
            break;
        }
    }
    }                 //switch_END
    return frameFlag; //������֡״̬
}

uint8_t is_equal(uint8_t* arr)
{
    for (uint8_t i = 0; i < g_uart_recvBuf[1]; i++)
    {
        if (arr[i] != g_uart_recvBuf[i + 2])
        {
            return 0;
        }
    }
    return 1;
}

/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
