//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"

void SUART_PortC_PortD_Handler(void) {

    uint32_t len = suart_receive(SUART_RX, gRecvBuff);
    printf("gLenth: %d\n", len);

    gI2CSendFlag = gRecvBuff[0] - '0';
}

void I2C_M0_Handler(void) {
    printf("send\n");
    static uint8_t isMe = 0;
    DISABLE_INTERRUPTS;     //�����ж�

    // ���isMe = 0; ������ж��Ƿ��Ǻ��Լ�ͨ��,
    // �����ַƥ�䣬˵���Ǻ��Լ�ͨ�ţ���isMe = 1
    if (!isMe) {
        // ��Ϊ��ַ�����ݷ���λ�ϲ���һ���ˣ������жϵ�ʱ��ҲҪ�ֽ���ַ�ͷ���ϲ����ж�
        if ((i2c_m0_addr << 1 & 0xFE) == i2c_read(I2C_M0)) {
            isMe = 1;
        }
    }
    else {
        uint32_t len = i2c_recv(I2C_M0, gRecvBuff);
        if (len) {
            suart_send(SUART_TX, gRecvBuff, strlen(gRecvBuff));
            isMe = 0;
        }
    }

    // ���ж�
    i2c_clear_re_int(I2C_M0);
    ENABLE_INTERRUPTS;      //�����ж�
}

void I2C_M1_Handler(void) {
    printf("receive\n");
    static uint8_t isMe = 0;
    DISABLE_INTERRUPTS;     //�����ж�

    // ���isMe = 0; ������ж��Ƿ��Ǻ��Լ�ͨ��,
    // �����ַƥ�䣬˵���Ǻ��Լ�ͨ�ţ���isMe = 1
    if (isMe == 0) {
        // ��Ϊ��ַ�����ݷ���λ�ϲ���һ���ˣ������жϵ�ʱ��ҲҪ�ֽ���ַ�ͷ���ϲ����ж�
        if ((i2c_m1_addr << 1 & 0xFE) == i2c_read(I2C_M1)) {
            isMe = 1;
        }
    }
    else {
        uint32_t len = i2c_recv(I2C_M1, gRecvBuff);
        if (len) {
            suart_send(SUART_TX, gRecvBuff, strlen(gRecvBuff));
            // ��������Ͻ��յ��ַ�������isMe = 0; ���������һ��ͨ���ж�
            isMe = 0;
        }

    }

    i2c_clear_re_int(I2C_M1);
    ENABLE_INTERRUPTS;      //�����ж�
}