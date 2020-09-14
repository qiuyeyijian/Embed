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
#include "gpio.h"


//（2）【变动】指示灯端口及引脚定义—根据实际使用的引脚改动
//指示灯端口及引脚定义
#define  LIGHT_RED    (PTA_NUM|5)  //红灯，所在引脚：GEC_56
#define  LIGHT_GREEN  (PTA_NUM|12)  //绿灯，所在引脚：GEC_55
#define  LIGHT_BLUE   (PTA_NUM|13)  //蓝灯，所在引脚：GEC_54

//灯状态宏定义（灯亮、灯暗对应的物理电平由硬件接法决定）
#define  LIGHT_ON       0    //灯亮
#define  LIGHT_OFF      1    //灯暗



#endif    //防止重复定义（USER_H 结尾）
