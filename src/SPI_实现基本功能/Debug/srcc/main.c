//====================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�2017.08, 2020.06
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//====================================================================

#define GLOBLE_VAR
#include "includes.h"      //������ͷ�ļ�

//----------------------------------------------------------------------
//����ʹ�õ����ڲ�����
//main.cʹ�õ��ڲ�����������
void delay_ms(uint32_t ms);
//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1.2�������䡿�����ж�
    DISABLE_INTERRUPTS;
    wdog_stop();

    uint8_t data[20] = "QIUYEYIJIAN";
    // ��ʼ��SPI���ͱ��־
    gSendFlag = 0;

    // ��ʼ�����ʹ��ںͽ��մ���
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);

    // ��SPI0��ʼ��Ϊ������������Ϊ6000�� ʱ�Ӽ���0����λ0
    spi_init(SPI_SEND, 1, 6000, 0, 0);
    // ��SPI1��ʼ��Ϊ������������Ϊ6000�� ʱ�Ӽ���0����λ0
    spi_init(SPI_RECEIVE, 0, 6000, 0, 0);



    // ʹ�ܴ��ڽ����ж�
    suart_enable_receive_interupt(SUART_RX);
    // ʹ��SPI1ģ���ж�
    spi_enable_re_int(SPI_RECEIVE);

    //��1.7�������䡿�����ж�
    ENABLE_INTERRUPTS;
    //��2��======��ѭ�����֣���β��========================================

    for (;;) {
        // delay_ms(1000);
        //SPI���ݷ���
        if (gSendFlag) {
            printf("�����ַ�����ʼ����!\n");
            spi_sendN(SPI_SEND, strlen(gRecvBuff), gRecvBuff);      //��������
            printf("�����ַ����������!\n");
            gSendFlag = 0;
        }
    }
}


void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (6000 * ms); i++) __asm("nop");
}



