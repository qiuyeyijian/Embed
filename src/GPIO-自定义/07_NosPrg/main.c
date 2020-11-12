//====================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：20170801-20200502
//功能描述：见本工程的<01_Doc>文件夹下Readme.txt文件
//====================================================================

#define GLOBLE_VAR
#include "includes.h" //包含总头文件

//----------------------------------------------------------------------
//声明使用到的内部函数
//main.c使用的内部函数声明处

//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    //1. 模块初始化
    light_init(LIGHT_RED, LIGHT_OFF);
    light_init(LIGHT_GREEN, LIGHT_OFF);
    light_init(LIGHT_BLUE, LIGHT_OFF);

    //【不变】开总中断
    ENABLE_INTERRUPTS;

    uint32_t loop_count = 0;
    volatile uint8_t flag = 0;

    for (;;) //for(;;)（开头）
    {
        if (loop_count++ < 10000000)
            continue;

        loop_count = 0;

        switch (flag)
        {
        case 1:

            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_ON);
            flag = 2;
            break;
        case 2:
            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_ON);
            flag = 3;
            break;

        case 3:
            light_control(LIGHT_RED, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_BLUE, LIGHT_ON);
            flag = 1;
            break;
        default:
            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_OFF);
            flag = 1;
            printf("default");
            break;
        }

    } //for(;;)结尾
}