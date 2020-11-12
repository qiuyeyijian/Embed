//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"
void SecAdd1(uint8_t* p);
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
	DISABLE_INTERRUPTS;      //�����ж�
	//------------------------------------------------------------------
	//����һ���ֽ�
	ch = uart_re1(UART_User, &flag);   //���ý���һ���ֽڵĺ�����������ж�λ
	if (flag)   //������
	{
		uart_send1(UART_User, ch);//�ط����յ����ֽ�
	}
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS;       //�����ж�
}

//=====================================================================
//�������ƣ�SYSTICK_USER_Handler��SysTick��ʱ���жϴ������
//����˵������
//�������أ���
//���ܸ�Ҫ����1��ÿ10ms�жϴ���������һ�Σ���2���ﵽһ��ʱ��������+1
//           ���򣬼��㡰ʱ���֡��롱
//�ر���ʾ����1��ʹ��ȫ�ֱ����ֽ�������gTime[3]���ֱ�洢��ʱ���֡��롱
//          ��2��ע�����о�̬������ʹ��
//=====================================================================
void SYSTICK_USER_Handler()
{
	static uint8_t SysTickCount = 0;
	SysTickCount++;    //Tick��Ԫ+1
	//wdog_feed();      //���Ź���ι����
	if (SysTickCount >= 100)
	{
		SysTickCount = 0;
		SecAdd1(gTime);

		uint8_t temp = gpio_get(s0) << 1 | gpio_get(s1);

		if (gStatus != temp) {
			printf("No: %d\n", temp);
			printf("No-gStatus: %d\n", gStatus);
			gFlag = 0;
			gStatus = temp;
		}
		else {
			printf("Yes-gStatus: %d\n", gStatus);
		}

		switch (gStatus)
		{
		case 1:
			printf("case 1 - gFlag: %d\n", gFlag);
			if (gFlag <= 3) {
				gpio_set(LIGHT_BLUE, LIGHT_ON);
			}
			if (gFlag > 3 && gFlag <= 5) {
				gpio_reverse(LIGHT_BLUE);
			}
			if (gFlag > 5) {
				gFlag = 0;
			}
			gFlag++;
			break;

		case 2:
			printf("case 2 - gFlag: %d\n", gFlag);
			if (gFlag <= 3) {
				gpio_set(LIGHT_BLUE, LIGHT_ON);
			}
			if (gFlag > 3 && gFlag <= 7) {
				gpio_reverse(LIGHT_BLUE);
			}
			if (gFlag > 7) {
				gFlag = 0;
			}
			gFlag++;
			break;

		case 3:
			printf("case 3 - gFlag: %d\n", gFlag);
			if (gFlag <= 3) {
				gpio_set(LIGHT_BLUE, LIGHT_ON);
			}
			if (gFlag > 3 && gFlag <= 9) {
				gpio_reverse(LIGHT_BLUE);
			}
			if (gFlag > 9) {
				gFlag = 0;
			}
			gFlag++;
			break;

		default:
			printf("default\n");
			gFlag = 0;
			gStatus = 0;
			break;
		}

	}
}



//===========================================================================
//�������ƣ�SecAdd1
//�������أ���
//����˵����*p:Ϊָ��һ��ʱ��������p[3]
//���ܸ�Ҫ���뵥Ԫ+1��������ʱ�ֵ�Ԫ��00:00:00-23:59:59)
//===========================================================================
void SecAdd1(uint8_t* p)
{
	*(p + 2) += 1;         //��+1
	if (*(p + 2) >= 60)     //�����
	{
		*(p + 2) = 0;       //����
		*(p + 1) += 1;      //��+1
		if (*(p + 1) >= 60)  //�����
		{
			*(p + 1) = 0;    //���
			*p += 1;       //ʱ+1
			if (*p >= 24)   //ʱ���
			{
				*p = 0;      //��ʱ
			}
		}
	}
}


/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
