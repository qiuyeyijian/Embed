//======================================================================
//�ļ����ƣ�user.h��userͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200221
//��Ҫ˵��������gpio.h�ļ���ʵ�ֶԾ���Ӧ�ñ�̣������Ƕ�����״̬�ȱ��
//======================================================================
#ifndef USER_H  //��ֹ�ظ����壨USER_H ��ͷ��
#define USER_H

//��1�����̶����ļ�����
#include "dma.h"
#include "gec.h"
#include "gpio.h"
#include "printf.h"
#include "uart.h"
#include "wdog.h"
//��2�����䶯��ָʾ�ƶ˿ڼ����Ŷ��塪����ʵ��ʹ�õ����ŸĶ�
//ָʾ�ƶ˿ڼ����Ŷ���
#define LIGHT_RED (PTA_NUM | 5)     //��ƣ��������ţ�GEC_56
#define LIGHT_GREEN (PTA_NUM | 12)  //�̵ƣ��������ţ�GEC_55
#define LIGHT_BLUE (PTA_NUM | 13)   //���ƣ��������ţ�GEC_54

//��״̬�궨�壨�������ư���Ӧ�������ƽ��Ӳ���ӷ�������
#define LIGHT_ON 0   //����
#define LIGHT_OFF 1  //�ư�

//��3�����䶯��UART����ģ�鶨��
#define UART_Debug 2  // UART_2   //���ڳ�����£��޷���ʹ��
#define UART_User 0   // UART_2   //�û�����
#define UART_UE 1     // UART_1   //�û�����
//��4�����Ķ����жϷ������궨��
#define UART_User_Handler UART0_IRQHandler  //�û������жϺ���
#define UART_UE_Handler UART1_IRQHandler
#define DMA_User_Handler DMA0_IRQHandler

/* the memory adress of UART0_D register*/
#define DMA_DST_ADDRESS (0x4006A007)

//��5��DMAͨ������
#define DMA_CH0 0  //ͨ��0
#define DMA_CH1 1  //ͨ��1
#define DMA_CH2 2  //ͨ��2
#define DMA_CH3 3  //ͨ��3

#define DMA_SEND DMA_CH0
#define DMA_RECV DMA_CH1

#endif  //��ֹ�ظ����壨USER_H ��β��
