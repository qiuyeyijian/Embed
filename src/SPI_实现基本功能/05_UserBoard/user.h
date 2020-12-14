//======================================================================
//文件名称：user.h（user头文件）
//制作单位：SD-Arm(sumcu.suda.edu.cn)
//更新记录：20181201-20200221
//概要说明：包含gpio.h文件，实现对具体应用编程，而不是对引脚状态等编程
//======================================================================
#ifndef USER_H   //防止重复定义（USER_H 开头）
#define USER_H

//（1）【固定】文件包含
#include "printf.h"
#include "gec.h"
#include "timer.h"
#include "incapture.h"
#include "pwm.h"
#include "wdog.h"
#include "suart.h"
#include "lcd.h"

//（2）【变动】指示灯端口及引脚定义—根据实际使用的引脚改动
//指示灯端口及引脚定义
#define  LIGHT_RED    (PTA_NUM|5)  //红灯，所在引脚：GEC_56
#define  LIGHT_GREEN  (PTA_NUM|12)  //绿灯，所在引脚：GEC_55
#define  LIGHT_BLUE   (PTA_NUM|13)  //蓝灯，所在引脚：GEC_54
#define  SPI_SEND     SPI_0
#define  SPI_RECEIVE  SPI_1


//【改动】中断服务函数宏定义
#define SUART_PortA_Handler PORTA_IRQHandler    //用户串口中断函数
#define SUART_PortC_PortD_Handler PORTC_PORTD_IRQHandler
#define SPI_Send_Handler    	SPI0_IRQHandler  //SPI0中断函数
#define SPI_Receive_Handler     SPI1_IRQHandler  //SPI1中断函数


#endif    //防止重复定义（USER_H 结尾）
