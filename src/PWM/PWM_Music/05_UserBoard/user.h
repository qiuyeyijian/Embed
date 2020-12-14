//======================================================================
//�ļ����ƣ�user.h��userͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200221
//��Ҫ˵��������gpio.h�ļ���ʵ�ֶԾ���Ӧ�ñ�̣������Ƕ�����״̬�ȱ��
//======================================================================
#ifndef USER_H  //��ֹ�ظ����壨USER_H ��ͷ��
#define USER_H

//��1�����̶����ļ�����
#include "gec.h"
#include "incapture.h"
#include "music.h"
#include "printf.h"
#include "pwm.h"
#include "suart.h"
#include "timer.h"
#include "wdog.h"

//��2�����䶯��ָʾ�ƶ˿ڼ����Ŷ��塪����ʵ��ʹ�õ����ŸĶ�
//ָʾ�ƶ˿ڼ����Ŷ���
#define LIGHT_RED (PTA_NUM | 5)     //��ƣ��������ţ�GEC_56
#define LIGHT_GREEN (PTA_NUM | 12)  //�̵ƣ��������ţ�GEC_55
#define LIGHT_BLUE (PTA_NUM | 13)   //���ƣ��������ţ�GEC_54
#define PWM_USER (PTC_NUM | 3)      //�û�PWM��� GEC_37
#define TIMER_USER TIMERB           //�û���ʱ��TPM1

#define UART_Debug 2  // UART_2   //���ڳ�����£��޷���ʹ��
#define UART_User 0   // UART_2   //�û�����
#define UART_UE 1     // UART_1   //�û�����

//���Ķ����жϷ������궨��
#define SUART_PortA_Handler PORTA_IRQHandler  //�û������жϺ���
#define SUART_PortC_PortD_Handler PORTC_PORTD_IRQHandler

#define TIMER_User_Handler TPM1_IRQHandler  //�û���ʱ���жϺ���
#define UART_User_Handler UART0_IRQHandler  //�û������жϺ���
#define UART_UE_Handler UART1_IRQHandler

#endif  //��ֹ�ظ����壨USER_H ��β��
