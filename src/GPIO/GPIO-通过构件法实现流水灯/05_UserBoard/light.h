#ifndef _LIGHT_H
#define _LIGHT_H

//头文件包含
//（1）【固定】文件包含
#include "printf.h"
#include "gec.h"
#include "gpio.h"

//（2）【变动】指示灯端口及引脚定义—根据实际使用的引脚改动
//指示灯端口及引脚定义
#define LIGHT_RED (PTA_NUM | 5)    //红灯，所在引脚：GEC_56
#define LIGHT_GREEN (PTA_NUM | 12) //绿灯，所在引脚：GEC_55
#define LIGHT_BLUE (PTA_NUM | 13)  //蓝灯，所在引脚：GEC_54

// 开关定义
#define SWITCH_0 (PTC_NUM | 0)
#define SWITCH_1 (PTC_NUM | 1)

// 下拉上拉
#define PULL_DOWN 0
#define PULL_UP 1

//灯状态宏定义（灯亮、灯暗对应的物理电平由硬件接法决定）
#define LIGHT_ON 0  //灯亮
#define LIGHT_OFF 1 //灯暗

// 初始化
void light_init(uint16_t port_pin, uint8_t state);

// 控制灯的亮暗
void light_control(uint16_t port_pin, uint8_t state);

//切换等的亮暗
void light_change(uint16_t port_pin);

#endif