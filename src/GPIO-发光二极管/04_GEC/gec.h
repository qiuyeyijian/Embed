//======================================================================
//�ļ����ƣ�gec.h��GECоƬ����ͷ�ļ���
//������λ���մ�arm��������(sumcu.suda.edu.cn)
//���¼�¼��2018.12-20200627
//======================================================================
#ifndef GEC_H    //��ֹ�ظ����壨GEC_H ��ʼ��
#define GEC_H
#include "mcu.h"
#include "printf.h"
#include "flash.h"

#include "includes.h"

//���䶯��GEC������Ϣ==================================================
#define GEC_USER_SECTOR_START    (26)  //26 18
#define GEC_COMPONENT_LST_START  (14)   //�����⺯���б�ʼ������  
#define BIOS_UART_UPDATE_IRQn    (30)      //BIOS����д�봮�ڵ��жϺţ�KL36����һ����
//���䶯����̬������ʼ��������������
#define GEC_DYNAMIC_START        (12)                                     
#define GEC_DYNAMIC_END	         (13) 
//���̶��������⺯��ָ���ϵͳ���ܺ�������===========================================
void **  component_fun;
void SYSTEM_FUNCTION(uint8_t * data);
#endif  //��ֹ�ظ����壨GEC_H ��β��
