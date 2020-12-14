//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"



void SUART_PortC_PortD_Handler(void) {

    uint32_t len = suart_receive(SUART_RX, gReceiveBuff);
    printf("gLenth: %d\n", len);

    uint8_t cmd = gReceiveBuff[0];
    uint16_t sect = gReceiveBuff[1];

    uint8_t i = 0;

    switch (cmd)
    {
    case '1':   // д�ļ�����
        flash_erase(sect);
        flash_write(sect, 0, 1024, gReceiveBuff + 2);
        break;
    case '2':   // ������
        flash_read_logic(gReceiveBuff, sect, 0, 1024);
        printf("��ȡ���ȣ�%d\n", strlen(gReceiveBuff));
        suart_send(SUART_TX, gReceiveBuff, strlen(gReceiveBuff));
        break;

    default:
        break;
    }

}