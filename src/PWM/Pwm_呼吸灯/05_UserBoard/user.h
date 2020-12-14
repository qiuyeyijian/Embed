//======================================================================
//�ļ����ƣ�user.h��userͷ�ļ���
//������λ��SD-Arm(sumcu.suda.edu.cn)
//���¼�¼��20181201-20200221
//��Ҫ˵��������gpio.h�ļ���ʵ�ֶԾ���Ӧ�ñ�̣������Ƕ�����״̬�ȱ��
//======================================================================
#ifndef USER_H   //��ֹ�ظ����壨USER_H ��ͷ��
#define USER_H

//��1�����̶����ļ�����
#include "printf.h"
#include "gec.h"
#include "gpio.h"
#include "timer.h"
#include "incapture.h"
#include "pwm.h"
#include "wdog.h"

//��2�����䶯��ָʾ�ƶ˿ڼ����Ŷ��塪����ʵ��ʹ�õ����ŸĶ�
//ָʾ�ƶ˿ڼ����Ŷ���
#define  LIGHT_RED    (PTA_NUM|5)  //��ƣ��������ţ�GEC_56
#define  LIGHT_GREEN  (PTA_NUM|12)  //�̵ƣ��������ţ�GEC_55
#define  LIGHT_BLUE   (PTA_NUM|13)  //���ƣ��������ţ�GEC_54

//��״̬�궨�壨�������ư���Ӧ�������ƽ��Ӳ���ӷ�������
#define  LIGHT_ON       0    //����
#define  LIGHT_OFF      1    //�ư�

//��3�����䶯��UART����ģ�鶨��
#define UART_Debug   2  //UART_2   //���ڳ�����£��޷���ʹ��
#define UART_User    0  //UART_2   //�û�����
#define UART_UE      1  //UART_1   //�û�����
//��4�����Ķ����жϷ������궨��
#define UART_User_Handler   UART0_IRQHandler  //�û������жϺ���
#define UART_UE_Handler UART1_IRQHandler
//��5�����Ķ�����ʱ������ģ�鶨��
#define PWM_USER   	 (PTC_NUM|1)  	//�û�PWM��� GEC_39��PTC_NUM|1��
#define INCAP_USER   (PTC_NUM|2)  	//�û�INCAP���벶׽ GEC_38��PTC_NUM|2��
#define TIMER_USER   TIMERB  //�û���ʱ��TPM1

#define TIMER_User_Handler   TPM1_IRQHandler    //�û���ʱ���жϺ���
#define INCAP_User_Handler   TPM0_IRQHandler    //�û����벶׽�жϺ���

#endif    //��ֹ�ظ����壨USER_H ��β��
