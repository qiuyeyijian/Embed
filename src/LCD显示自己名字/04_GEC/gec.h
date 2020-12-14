//======================================================================
//文件名称：gec.h（GEC芯片引脚头文件）
//制作单位：苏大arm技术中心(sumcu.suda.edu.cn)
//更新记录：2018.12-20200627
//======================================================================
#ifndef GEC_H    //防止重复定义（GEC_H 开始）
#define GEC_H
#include "mcu.h"
#include "printf.h"
#include "flash.h"

#include "includes.h"

//【变动】GEC基本信息==================================================
#define GEC_USER_SECTOR_START    (26)  //26 18
#define GEC_COMPONENT_LST_START  (14)   //构件库函数列表开始扇区号  
#define BIOS_UART_UPDATE_IRQn    (30)      //BIOS程序写入串口的中断号（KL36，仅一处）
//【变动】动态命令起始扇区、结束扇区
#define GEC_DYNAMIC_START        (12)                                     
#define GEC_DYNAMIC_END	         (13) 
//【固定】构件库函数指针和系统功能函数声明===========================================
void **  component_fun;
void SYSTEM_FUNCTION(uint8_t * data);
#endif  //防止重复定义（GEC_H 结尾）
