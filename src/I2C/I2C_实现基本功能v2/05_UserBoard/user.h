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
#include "timer.h"
#include "incapture.h"
#include "pwm.h"
#include "wdog.h"
#include "suart.h"
#include "i2c.h"

//��2�����䶯��ָʾ�ƶ˿ڼ����Ŷ��塪����ʵ��ʹ�õ����ŸĶ�
//ָʾ�ƶ˿ڼ����Ŷ���
#define  LIGHT_RED    (PTA_NUM|5)  //��ƣ��������ţ�GEC_56
#define  LIGHT_GREEN  (PTA_NUM|12)  //�̵ƣ��������ţ�GEC_55
#define  LIGHT_BLUE   (PTA_NUM|13)  //���ƣ��������ţ�GEC_54
//I2Cģ�鶨��
#define I2C_M0     0  //SDA���ţ�GEC_46��PTB_NUM|3����SCL���ţ� GEC_47��PTB_NUM|2��
#define I2C_M1  1  //SDA���ţ�GEC_38��PTC_NUM|2����SCL���ţ� GEC_39��PTC_NUM|1��



//���Ķ����жϷ������궨��
#define SUART_PortA_Handler PORTA_IRQHandler    //�û������жϺ���
#define SUART_PortC_PortD_Handler PORTC_PORTD_IRQHandler
#define I2C_M0_Handler    	I2C0_IRQHandler  //I2C0�жϺ���
#define I2C_M1_Handler     I2C1_IRQHandler  //I2C1�жϺ���


#endif    //��ֹ�ظ����壨USER_H ��β��
