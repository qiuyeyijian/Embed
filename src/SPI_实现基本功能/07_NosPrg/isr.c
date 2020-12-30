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
  gSendFlag = 1;
}

void SPI_Send_Handler(void) {
  DISABLE_INTERRUPTS;  //�����ж�
  //------------------------------------------------------------------
  //���ڴ˴����ӹ��ܣ�
  uint32_t len = spi_receiveN(SPI_RECEIVE, gRecvBuff);
  // ���len�ĳ��Ȳ�Ϊ0,˵���ַ����ճɹ�������Ϊlen
  if (len) {
    printf("%s\n", gRecvBuff);
    suart_send(SUART_TX, gRecvBuff, len);
  }

  //------------------------------------------------------------------
  ENABLE_INTERRUPTS;  //�����ж�
}

void SPI_Receive_Handler(void) {
  DISABLE_INTERRUPTS;  //�����ж�
  //------------------------------------------------------------------
  //���ڴ˴����ӹ��ܣ�
  uint32_t len = spi_receiveN(SPI_RECEIVE, gRecvBuff);
  // ���len�ĳ��Ȳ�Ϊ0,˵���ַ����ճɹ�������Ϊlen
  if (len) {
    printf("%s\n", gRecvBuff);
    suart_send(SUART_TX, gRecvBuff, len);
  }

  //------------------------------------------------------------------
  ENABLE_INTERRUPTS;  //�����ж�
}
