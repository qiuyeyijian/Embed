//============================================================================
//文件名称：dac.c
//功能概要：KL36 DAC底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2020-11-19 V2.0    
//============================================================================

#ifndef _DAC_H          //防止重复定义（ 开头）
#define _DAC_H

#include "mcu.h"
#include "string.h"


// #include "common.h"   //包含公共要素头文件

#define DAC_VREFH 0
#define DAC_VDDA  1

//============================================================================
//函数名称：dac_init
//函数返回：无
//参数说明：RefVoltage：参考电压选择 DAC_VREFH或 DAC_VDDA
//功能概要：初始化DAC模块设定
//============================================================================
void dac_init(uint8_t RefVoltage);

//============================================================================
//函数名称：dac_convert
//参数说明：data: 需要转换成模拟量的数字量，范围（0~4095）
//功能概要：执行DAC转换
//============================================================================   
void dac_convert(uint16_t data);


#endif

//===========================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学飞思卡尔嵌入式中心（苏州华祥信息科技有限公司）
//技术咨询：0512-65214835  http://sumcu.suda.edu.cn
//业务咨询：0512-87661670,18915522016  http://www.hxtek.com.cn
