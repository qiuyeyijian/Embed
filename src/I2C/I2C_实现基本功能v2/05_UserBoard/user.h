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
#include "i2c.h"

//（2）【变动】指示灯端口及引脚定义—根据实际使用的引脚改动
//指示灯端口及引脚定义
#define  LIGHT_RED    (PTA_NUM|5)  //红灯，所在引脚：GEC_56
#define  LIGHT_GREEN  (PTA_NUM|12)  //绿灯，所在引脚：GEC_55
#define  LIGHT_BLUE   (PTA_NUM|13)  //蓝灯，所在引脚：GEC_54
//I2C模块定义
#define I2C_M0     0  //SDA引脚：GEC_46（PTB_NUM|3）；SCL引脚： GEC_47（PTB_NUM|2）
#define I2C_M1  1  //SDA引脚：GEC_38（PTC_NUM|2）；SCL引脚： GEC_39（PTC_NUM|1）



//【改动】中断服务函数宏定义
#define SUART_PortA_Handler PORTA_IRQHandler    //用户串口中断函数
#define SUART_PortC_PortD_Handler PORTC_PORTD_IRQHandler
#define I2C_M0_Handler    	I2C0_IRQHandler  //I2C0中断函数
#define I2C_M1_Handler     I2C1_IRQHandler  //I2C1中断函数


#endif    //防止重复定义（USER_H 结尾）
