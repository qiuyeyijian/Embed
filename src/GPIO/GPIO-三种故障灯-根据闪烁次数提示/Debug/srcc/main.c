//====================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：20170801-20200502
//功能描述：见本工程的<01_Doc>文件夹下Readme.txt文件
//====================================================================

#define GLOBLE_VAR
#include "includes.h"      //包含总头文件

//----------------------------------------------------------------------
//声明使用到的内部函数
//main.c使用的内部函数声明处
//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //（1）======启动部分（开头）==========================================
    //（1.1）声明main函数使用的局部变量
    uint32_t mMainLoopCount;  //主循环使用的记录主循环次数变量
    uint8_t  mFlag;            //主循环使用的临时变量

    //（1.2）【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    //（1.3）给主函数使用的局部变量赋初值
    mMainLoopCount = 0;
    mFlag = 3;

    //（1.4）给全局变量赋初值

    //（1.5）用户外设模块初始化
    light_init(LIGHT_RED, LIGHT_OFF);    //红灯初始化
    gpio_init(s0, GPIO_INPUT, 1);
    gpio_init(s1, GPIO_INPUT, 1);

    gpio_pull(s0, 0);
    gpio_pull(s1, 0);


    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;

    uint32_t frequency = 5000000;
    uint32_t number = 1;

    //（2）======主循环部分（开头）========================================
    for (;;)     //for(;;)（开头）
    {
        // if (mMainLoopCount++ < 5000000) continue;
        // mMainLoopCount = 0;

        mFlag = gpio_get(s0) + gpio_get(s1);

        printf("\nFlag: %d \n", mFlag);

        switch (mFlag)
        {
        case 0:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 6);
            printf("\n故障一 ");
            break;
        case 1:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 4);
            printf("\n故障二 ");
            break;
        case 2:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 6);
            break;
        default:
            light_set(LIGHT_RED, LIGHT_OFF);
            printf("\n中间状态");
            break;
        }
    }     //for(;;)结尾
    //（2）======主循环部分（结尾）=====================================
}

//======以下为主函数调用的子函数存放处===================================

//======================================================================
/*
知识要素：
（1）main.c是一个模板，该文件所有代码均不涉及具体的硬件和环境，通过调用构件
实现对硬件的干预。
（2）本文件中标有【不变】的地方为系统保留，此类代码与具体项目无关，不宜删除。
（3）本文件中对宏GLOBLE_VAR进行了定义，所以在包含"includes.h"头文件时，会定
义全局变量，在其他文件中包含"includes.h"头文件时，
编译时会自动增加extern
*/