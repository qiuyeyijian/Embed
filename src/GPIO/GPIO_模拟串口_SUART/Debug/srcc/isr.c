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

    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[0] >> i & 1);
    // }
    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[1] >> i & 1);
    // }
    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[2] >> i & 1);
    // }
    // printf("=====================\n");



    // for (uint32_t i = 0; i < len; i++) {
    //     printf("%c", gReceiveBuff[i]);
    // }

    suart_send(SUART_TX, gReceiveBuff, len);

}