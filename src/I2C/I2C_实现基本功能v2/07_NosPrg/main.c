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

    uint8_t data[11] = "QIUYEYIJIAN";
    gI2CSendFlag = 0;
    i2c_m0_addr = 0x74;
    i2c_m1_addr = 0x73;


    //��1.2�������䡿�����ж�
    DISABLE_INTERRUPTS;
    wdog_stop();

    // ��ʼ�����ʹ��ںͽ��մ���
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);

    //I2C_M0��ʼ����I2C_M0:ģ��� 1:���� 0x74:��ַ  100:������(��λ:KBps)
    i2c_init(I2C_M0, 0, i2c_m0_addr, 100);
    //I2C_M1��ʼ����I2C_M1:ģ��� 0:�ӻ� 0x73:��ַ  100:������(��λ:KBps)
    i2c_init(I2C_M1, 0, i2c_m1_addr, 100);

    // ʹ�ܴ��ڽ����ж�
    suart_enable_receive_interupt(SUART_RX);
    // ��ֹi2c�����ж�
    i2c_enable_re_int(I2C_M0);
    i2c_enable_re_int(I2C_M1);


    //��1.7�������䡿�����ж�
    ENABLE_INTERRUPTS;
    //��2��======��ѭ�����֣���β��========================================
    for (;;) {
        delay_ms(1000);

        if (gI2CSendFlag == 1) {
            i2c_send(I2C_M0, i2c_m1_addr, gRecvBuff, strlen(gRecvBuff));
            gI2CSendFlag = 0;
            printf("1111\n");

        }
        else if (gI2CSendFlag == 2) {
            i2c_send(I2C_M1, i2c_m0_addr, gRecvBuff, strlen(gRecvBuff));
            gI2CSendFlag = 0;
            printf("2222\n");
        }


        printf("Hello, World!\n");
    }
}

// ��ʱ����
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (6000 * ms); i++) {
        __asm("nop");
    }
}



