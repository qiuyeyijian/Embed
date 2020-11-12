//=====================================================================
//文件名称：isr.c（中断处理程序源文件）
//框架提供：SD-ARM（sumcu.suda.edu.cn）
//版本更新：20170801-20191020
//功能描述：提供中断处理程序编程框架
//=====================================================================
#include "includes.h"
//======================================================================
//中断服务程序名称：UART_USER_Handler
//触发条件：UART_USE串口收到一个字节触发
//功    能：收到一个字节，直接返回该字节
//备    注：进入本程序后，可使用uart_get_re_int函数可再进行中断标志判断
//          （1-有UART接收中断，0-没有UART接收中断）
//======================================================================
void UART_User_Handler(void)
{
	uint8_t ch;
	uint8_t flag;
	uint8_t buff[5];
	DISABLE_INTERRUPTS; //关总中断
	//------------------------------------------------------------------
	//接收一个字节
	flag = uart_reN(UART_User, 5, buff);
	// ch = uart_re1(UART_User, &flag); //调用接收一个字节的函数，清接收中断位
	if (flag) //有数据
	{

		if (buff[0] == 'o' && buff[1] == 'p' && buff[2] == 'e' && buff[3] == 'n')
		{
			uart_send_string(UART_User, "Open!\n");
			gpio_set(LIGHT_BLUE, LIGHT_ON);
		}
		if (buff[0] == 'c' && buff[1] == 'l' && buff[2] == 'o' && buff[3] == 's' && buff[4] == 'e')
		{
			uart_send_string(UART_User, "Close!\n");
			gpio_set(LIGHT_BLUE, LIGHT_OFF);
		}
	}
	// else
	// {
	// 	uart_send_string(UART_User, "Please send: (open | close)!\n");
	// }
	//------------------------------------------------------------------
	ENABLE_INTERRUPTS; //开总中断
}

/*
 知识要素：
 1.本文件中的中断处理函数调用的均是相关设备封装好的具体构件，在更换芯片
 时，只需保证设备的构件接口一致，即可保证本文件的相关中断处理函数不做任何
 更改，从而达到芯片无关性的要求。
 */
