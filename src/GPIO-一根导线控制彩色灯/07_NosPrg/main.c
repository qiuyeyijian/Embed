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
    gpio_init(LIGHT_BLUE, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(SWITCH_0, GPIO_INPUT, LIGHT_OFF);

    //2. 设置下拉引脚
    gpio_pull(SWITCH_0, PULL_DOWN);

    //【不变】开总中断
    ENABLE_INTERRUPTS;

    for (;;) //for(;;)（开头）
    {
        // 取相应引脚的输入状态
        volatile uint8_t portC_pin_0_state = gpio_get(SWITCH_0);

        printf("input, %d", portC_pin_0_state);

        if (portC_pin_0_state)
        {
            gpio_set(LIGHT_BLUE, LIGHT_ON);
            printf("\n 蓝灯亮");
        }
        else
        {
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            printf("\n 蓝灯灭");
        }

    } //for(;;)结尾
}