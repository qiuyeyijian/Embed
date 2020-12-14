//======================================================================
//文件名称：includes.h（应用工程总头文件）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：20170801-20191020
//功能描述：包含用到的构件、宏常数定义、自定义数据类型、声明全局变量。
//======================================================================

#ifndef _INCLUDES_H  //防止重复定义（开头）
#define _INCLUDES_H

//----------------------------------------------------------------------
//（1）包含用到的构件
#include "user.h"

//----------------------------------------------------------------------
//（2）全局使用的宏常数。

//----------------------------------------------------------------------
//（3）自定义数据类型

//----------------------------------------------------------------------

//（4）声明全局变量。命名规范见注2。
//【不动】宏定义全局变量前缀G_VAR_PREFIX。实现原理见注3。
#ifdef GLOBLE_VAR            // GLOBLE_VAR在main.c文件中有宏定义
#define G_VAR_PREFIX         //前缀G_VAR_PREFIX定义为空
#else                        // GLOBLE_VAR在非main.c文件中无定义
#define G_VAR_PREFIX extern  //前缀G_VAR_PREFIX定义为"extern"
#endif
//（在此增加全局变量）
G_VAR_PREFIX uint8_t gRecvBuff[1030];
G_VAR_PREFIX uint8_t gSendBuff[512];
G_VAR_PREFIX uint8_t gFlag;
G_VAR_PREFIX uint32_t gLen;
//----------------------------------------------------------------------

#endif  //防止重复定义（结尾）

/*
 知识要素：
 注1：全局使用的宏常数，使用全大写
 注2：全局变量命名一律以g开头，尽可能保证全工程内唯一性，并且一定要注明
      其含义，（声明时不准赋值）
 注3：全局变量一处声明多处使用处理方法代码段是为了解决全局变量重复声明
 的问题，宏GLOBLE_VAR仅在main.c中声明，所以只有在main.c中包含
 本头文件时才会声明全局变量，在其他文件中包含本文件时，编译时，就会自动加
 上前缀extern，表明是main函数中声明的全局变量，该文件中直接使用。
 */
