//===================================================================
//文件名称：light.h
//功能概要：故障灯构件头文件
//设计者：1813021018-沈晔-计181
//更新记录：2020/9/27
//===================================================================

# ifndef LIGHT_H   //防止重复定义（LIGHT_H 开头）
# define LIGHT_H

#include "printf.h"
#include "gec.h"
#include "gpio.h"


void light_delay(uint32_t delay_time);

//==============================接口函数说明===========================
//函数名称：light_init
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//         state：设定小灯状态，由宏定义。
//功能概要：指示灯驱动初始化
//=====================================================================
void light_init(uint16_t port_pin, uint8_t state);

//=====================================================================
//函数名称：light_set
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//          state：设定小灯状态，由宏定义。
//功能概要：控制小灯亮暗。
//=====================================================================
void light_set(uint16_t port_pin, uint8_t state);

//=====================================================================
//函数名称：light_flash
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//          state：设定小灯状态，由宏定义。
//          mMainLoopCount：小灯持续时间。
//功能概要：控制小灯闪烁。
//=====================================================================
void light_flash(uint16_t port_pin, uint32_t frequency, uint8_t number);

//=====================================================================
//函数名称：light_reverse
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PTB_NUM)|(9) 表示为B口9号脚）
//功能概要：切换指示灯状态
//=====================================================================
void light_reverse(uint16_t port_pin);
#endif    //防止重复定义（LIGHT_H 结尾）