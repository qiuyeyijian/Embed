//=====================================================================
//�ļ����ƣ�dma.h
//���ܸ�Ҫ��dma�ײ���������ͷ�ļ�
//������λ����ͨ��ѧ ��181������
//��    ����  2020-12-31 V2.0
//����оƬ��KL36
//=====================================================================

#ifndef _DMA_H_
#define _DMA_H_
#include "mcu.h"  //��������Ҫ��ͷ�ļ�

// DMAģʽ
#define DMA_MODE_NORMAl 0    //��ͨģʽ
#define DMA_MODE_PERIODIC 1  //����ģʽ
// DMA������
#define DMA_TRANS_WIDTH_BIT32 (0x00)  // DMA������Ϊ32λ
#define DMA_TRANS_WIDTH_BIT8 (0x01)   // DMA������Ϊ8λ
#define DMA_TRANS_WIDTH_BIT16 (0x02)  // DMA������Ϊ16λ
// DMAԴ��ַģ����С
#define DMA_MODULE_DISABLE (0)  //����ģ��
#define DMA_MODULO_BYTE_16 (1)
#define DMA_MODULO_BYTE_32 (2)
#define DMA_MODULO_BYTE_64 (3)
#define DMA_MODULO_BYTE_128 (4)
#define DMA_MODULO_BYTE_256 (5)
#define DMA_MODULO_BYTE_512 (6)
#define DMA_MODULO_BYTE_1K (7)
#define DMA_MODULO_BYTE_2K (8)
#define DMA_MODULO_BYTE_4K (9)
#define DMA_MODULO_BYTE_8K (10)
#define DMA_MODULO_BYTE_16K (11)
#define DMA_MODULO_BYTE_32K (12)
#define DMA_MODULO_BYTE_64K (13)
#define DMA_MODULO_BYTE_128K (14)
#define DMA_MODULO_BYTE_256K (15)
//Դ��ַƫ��������
#define DMA_SRC_ADDR_OFFSET_ON (1)
#define DMA_SRC_ADDR_OFFSET_OFF (0)
//Ŀ���ַƫ��������
#define DMA_DST_ADDR_OFFSET_ON (1)
#define DMA_DST_ADDR_OFFSET_OFF (0)
// �����ֽ��������Ĵ������ֵ
#define DMA_DSR_BCR_MAXLEN (65535)
// DMA����Դ
typedef enum DMA_sources {
  Channel_Disabled = 0,
  DMA_UART0_Rx = 2,
  DMA_UART0_Tx = 3,
  DMA_UART1_Rx = 4,
  DMA_UART1_Tx = 5,
  DMA_UART2_Rx = 6,
  DMA_UART2_Tx = 7,
  /*        I2S            */
  DMA_I2S0_Rx = 14,
  DMA_I2S0_Tx = 15,
  /*        SPI            */
  DMA_SPI0_Rx = 16,
  DMA_SPI0_Tx = 17,
  DMA_SPI1_Rx = 18,
  DMA_SPI1_Tx = 19,
  /*        I2C            */
  DMA_I2C0 = 22,
  DMA_I2C1 = 23,
  /*        TPM            */
  DMA_TPM0_CH0 = 24,
  DMA_TPM0_CH1 = 25,
  DMA_TPM0_CH2 = 26,
  DMA_TPM0_CH3 = 27,
  DMA_TPM0_CH4 = 28,
  DMA_TPM0_CH5 = 29,
  DMA_TPM1_CH0 = 32,
  DMA_TPM1_CH1 = 33,
  DMA_TPM2_CH0 = 34,
  DMA_TPM2_CH1 = 35,
  /*     ADC/DAC/CMP    */
  DMA_ADC0 = 40,
  DMA_CMP0 = 42,
  DMA_DAC0 = 45,
  DMA_Port_A = 49,
  DMA_Port_C = 51,
  DMA_Port_D = 52,
  DMA_TPM0_OVERFLOW = 54,
  DMA_TPM1_OVERFLOW = 55,
  DMA_TPM2_OVERFLOW = 56,
  DMA_TSI = 57,
  DMA_Always_EN1 = 60,
  DMA_Always_EN2 = 61,
  DMA_Always_EN3 = 62,
  DMA_Always_EN4 = 63,
} DMA_sources;

//����DMAͨ��״̬
typedef enum {
  DMAChannelIdle,       //ͨ������
  DMAChannelStarting,   //ͨ����������
  DMAChannelExecuting,  //ͨ������ִ��
  DMAChannelDone        //ͨ�������ѭ��
} DMAChannelState;

//==========================================================================
//��������: dma_uart_init
//��������: ��
//����˵��:  chSend: DMA����ͨ��
//         chRecv: DMA����ͨ��
//         buff: DMA���ڴ��еĻ�����
//���ܸ�Ҫ: ��ʼ��UART��DMA����
//==========================================================================
void dma_uart_init(uint8_t chSend, uint8_t chRecv, uint32_t buff);

//==========================================================================
//��������: dma_uart_recv
//��������: ��
//����˵��:  chRecv: DMA����ͨ��
//         dstAddress: Ŀ�ĵ�ַ��Դ��ַ�̶�ΪUART0�����ݼĴ�����ַ
//         len: ���յ����ݳ���
//���ܸ�Ҫ:  ʹ��һ��DMA���մ���
//==========================================================================
void dma_uart_recv(uint8_t chRecv, uint32_t dstAddress, uint32_t len);

//==========================================================================
//��������: dma_uart_send
//��������: ��
//����˵��:  chSend: DMA����ͨ��
//         srcAddress: Դ��ַ��Ŀ�ĵ�ַ�̶�ΪUART0�����ݼĴ�����ַ
//         len: ���յ����ݳ���
//���ܸ�Ҫ:  ʹ��һ��DMA���ʹ���
//==========================================================================
void dma_uart_send(uint8_t chSend, uint32_t srcAddress, uint32_t len);

//==========================================================================
//��������: dma_get_recv_count
//��������: ��
//����˵��:  chRecv: DMA����ͨ��
//���ܸ�Ҫ:  ��ȡDMA����ͨ���������ݸ���
//==========================================================================
uint32_t dma_get_recv_count(uint8_t chRecv);

//==========================================================================
//��������: dma_enable_int
//��������: ��
//����˵��:  ch: DMAͨ��
//���ܸ�Ҫ:  ʹ��DMAͨ���ж�
//==========================================================================
void dma_enable_int(uint8_t ch);

//==========================================================================
//��������: dma_clear
//��������: ��
//����˵��:  ch: DMAͨ��
//���ܸ�Ҫ:  ��ͨ���ж�������־
//==========================================================================
void dma_clear(uint8_t ch);

#endif
