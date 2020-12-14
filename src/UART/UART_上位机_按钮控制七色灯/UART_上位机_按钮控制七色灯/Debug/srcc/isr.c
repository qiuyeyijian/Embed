//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"
//======================================================================
//�жϷ���������ƣ�UART_USER_Handler
//����������UART_USE�����յ�һ���ֽڴ���
//��    �ܣ��յ�һ���ֽڣ�ֱ�ӷ��ظ��ֽ�
//��    ע�����뱾����󣬿�ʹ��uart_get_re_int�������ٽ����жϱ�־�ж�
//          ��1-��UART�����жϣ�0-û��UART�����жϣ�
//======================================================================
void UART_User_Handler(void)
{
	uint8_t ch;
	uint8_t flag;
	DISABLE_INTERRUPTS; //�����ж�
	//------------------------------------------------------------------
	//����һ���ֽ�
	ch = uart_re1(UART_User, &flag); //���ý���һ���ֽڵĺ�����������ж�λ
	if (flag)						 //������
	{
		switch (ch)
		{
		case '1':	//���
			gpio_set(LIGHT_RED, LIGHT_ON);
			gpio_set(LIGHT_GREEN, LIGHT_OFF);
			gpio_set(LIGHT_BLUE, LIGHT_OFF);
			printf("case 1");
			break;
		case '2':	//�̵�
			gpio_set(LIGHT_RED, LIGHT_OFF);
			gpio_set(LIGHT_GREEN, LIGHT_ON);
			gpio_set(LIGHT_BLUE, LIGHT_OFF);
			printf("case 2");
			break;
		case '3':	// ����
			gpio_set(LIGHT_BLUE, LIGHT_ON);
			gpio_set(LIGHT_GREEN, LIGHT_OFF);
			gpio_set(LIGHT_RED, LIGHT_OFF);
			printf("case 3");
			break;
		case '4':	//�Ƶ�
			gpio_set(LIGHT_GREEN, LIGHT_ON);
			gpio_set(LIGHT_RED, LIGHT_ON);
			gpio_set(LIGHT_BLUE, LIGHT_OFF);
			break;
		case '5':	//���
			gpio_set(LIGHT_GREEN, LIGHT_ON);
			gpio_set(LIGHT_RED, LIGHT_OFF);
			gpio_set(LIGHT_BLUE, LIGHT_ON);
			break;
		case '6':	// �ϵ�
			gpio_set(LIGHT_GREEN, LIGHT_OFF);
			gpio_set(LIGHT_RED, LIGHT_ON);
			gpio_set(LIGHT_BLUE, LIGHT_ON);
			break;
		case '7':	// �׵�
			gpio_set(LIGHT_GREEN, LIGHT_ON);
			gpio_set(LIGHT_RED, LIGHT_ON);
			gpio_set(LIGHT_BLUE, LIGHT_ON);
			break;
		case '0':
			gpio_set(LIGHT_RED, LIGHT_OFF);
			gpio_set(LIGHT_GREEN, LIGHT_OFF);
			gpio_set(LIGHT_BLUE, LIGHT_OFF);
			printf("case 0");
			break;
		default:
			printf("default!\n");
			break;
		}
		uart_send1(UART_User, ch); //�ط����յ����ֽ�
	}
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS; //�����ж�
}

/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
