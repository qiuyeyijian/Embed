//=====================================================================
//�ļ����ƣ�isr.c���жϴ������Դ�ļ���
//����ṩ��SD-ARM��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//�����������ṩ�жϴ�������̿��
//=====================================================================
#include "includes.h"
uint8_t CreateFrame(uint8_t Data, uint8_t* buffer);
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
	ch = uart_re1(UART_User, &flag); //���ý���һ���ֽڵĺ�����������ж�λ
	//�����ڲ�����CreateFrame������֡
	if (CreateFrame(ch, g_uart_recvBuf) != 0) //��֡�ɹ�
	{
		// P812:00:00C
		gTime[0] = (g_uart_recvBuf[2] - 48) * 10 + (g_uart_recvBuf[3] - 48);	// ʱ
		gTime[1] = (g_uart_recvBuf[5] - 48) * 10 + (g_uart_recvBuf[6] - 48);	// ��
		gTime[2] = (g_uart_recvBuf[8] - 48) * 10 + (g_uart_recvBuf[9] - 48);	// ��

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

		if (gTime[2] % 3 == 0) {
			gpio_reverse(LIGHT_BLUE);
		}

		char sendData[9];
		uint8_t index = 0;

		for (int i = 0; i < 3; i++) {
			sendData[index++] = (char)(gTime[i] / 10 + 48);
			sendData[index++] = (char)(gTime[i] % 10 + 48);
			// if (gTime[i] > 9) {
			// 	sendData[index++] = (char)(gTime[i] / 10 + 48);
			// 	sendData[index++] = (char)(gTime[i] % 10 + 48);
			// }
			// else {
			// 	sendData[index++] = '0';
			// 	sendData[index++] = (char)(gTime[i] + 48);
			// }
			if (i < 2) {
				sendData[index++] = ':';
			}
		}

		//�ַ����ԡ�\0����β
		sendData[8] = '\0';
		uart_send_string(UART_User, sendData);
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

//===========================================================================
//�������ƣ�CreateFrame
//���ܸ�Ҫ���齨����֡��������֡���ݼ��뵽����֡��
//����˵����Data��                   ����֡����
//          buffer:        ����֡����
//�������أ���֡״̬    0-��֡δ�ɹ���1-��֡�ɹ�
//��ע��ʮ����������֡��ʽ
//               ֡ͷ        + ���ݳ���         + ��Ч����    +  ֡β
//           FrameHead   +   len       + ��Ч����    + FrameTail
//===========================================================================

#define FrameHead (0x50) //֡ͷ     ASCII���ӦP
#define FrameTail (0x43) //֡β     ASCII���ӦC
//P4openC
uint8_t CreateFrame(uint8_t Data, uint8_t* buffer)
{
	static uint8_t frameLen = 0; //֡�ļ�����
	uint8_t frameFlag;           //��֡״̬

	frameFlag = 0; //��֡״̬��ʼ��
	//���ݾ�̬����frameCount��֡
	switch (frameLen)
	{
	case 0: //��һ������
	{
		if (Data == FrameHead) //�յ�������֡ͷFrameHead
		{
			buffer[0] = Data;
			frameLen++;
			frameFlag = 0; //��֡��ʼ
		}
		break;
	}
	case 1: //�ڶ������ݣ��������������յ����ݸ���
	{
		buffer[1] = Data - 0x30;
		frameLen++;
		break;
	}
	default: //�������
	{
		//�ڶ�λ��������Ч���ݳ���,�������������µ�����ֱ��֡βǰһλ
		if (frameLen >= 2 && frameLen <= (buffer[1] + 1))
		{
			buffer[frameLen] = Data;
			frameLen++;
			break;
		}

		//����ĩβ������ִ��
		if (frameLen >= (buffer[1] + 2))
		{
			if (Data == FrameTail) //����֡β
			{
				buffer[frameLen] = Data; //��֡β���뻺����
				frameFlag = 1;           //��֡�ɹ�
			}
			frameLen = 0; //������0��׼��������֡
			break;
		}
	}
	}                 //switch_END
	return frameFlag; //������֡״̬
}



/*
 ֪ʶҪ�أ�
 1.���ļ��е��жϴ��������õľ�������豸��װ�õľ��幹�����ڸ���оƬ
 ʱ��ֻ�豣֤�豸�Ĺ����ӿ�һ�£����ɱ�֤���ļ�������жϴ����������κ�
 ���ģ��Ӷ��ﵽоƬ�޹��Ե�Ҫ��
 */
