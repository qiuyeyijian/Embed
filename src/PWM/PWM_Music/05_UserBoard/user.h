//======================================================================
//文件名称：user.h（user头文件）
//制作单位：SD-Arm(sumcu.suda.edu.cn)
//更新记录：20181201-20200221
//概要说明：包含gpio.h文件，实现对具体应用编程，而不是对引脚状态等编程
//======================================================================
#ifndef USER_H  //防止重复定义（USER_H 开头）
#define USER_H

//（1）【固定】文件包含
#include "gec.h"
#include "incapture.h"
#include "music.h"
#include "printf.h"
#include "pwm.h"
#include "suart.h"
#include "timer.h"
#include "wdog.h"

//（2）【变动】指示灯端口及引脚定义—根据实际使用的引脚改动
//指示灯端口及引脚定义
#define LIGHT_RED (PTA_NUM | 5)     //红灯，所在引脚：GEC_56
#define LIGHT_GREEN (PTA_NUM | 12)  //绿灯，所在引脚：GEC_55
#define LIGHT_BLUE (PTA_NUM | 13)   //蓝灯，所在引脚：GEC_54
#define PWM_USER (PTC_NUM | 3)      //用户PWM输出 GEC_37
#define TIMER_USER TIMERB           //用户定时器TPM1

#define UART_Debug 2  // UART_2   //用于程序更新，无法被使用
#define UART_User 0   // UART_2   //用户串口
#define UART_UE 1     // UART_1   //用户串口

//【改动】中断服务函数宏定义
#define SUART_PortA_Handler PORTA_IRQHandler  //用户串口中断函数
#define SUART_PortC_PortD_Handler PORTC_PORTD_IRQHandler

#define TIMER_User_Handler TPM1_IRQHandler  //用户定时器中断函数
#define UART_User_Handler UART0_IRQHandler  //用户串口中断函数
#define UART_UE_Handler UART1_IRQHandler

#endif  //防止重复定义（USER_H 结尾）
