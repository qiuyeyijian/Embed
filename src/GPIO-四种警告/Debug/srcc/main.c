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

void light_flash(uint16_t port_pin, uint32_t delay_time);

//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    //1. 模块初始化
    gpio_init(LIGHT_RED, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(LIGHT_GREEN, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(LIGHT_BLUE, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(SWITCH_0, GPIO_INPUT, LIGHT_OFF);
    gpio_init(SWITCH_1, GPIO_INPUT, LIGHT_OFF);

    //2. 引脚下拉
    gpio_pull(SWITCH_0, PULL_DOWN);
    gpio_pull(SWITCH_1, PULL_DOWN);

    //

    //【不变】开总中断
    ENABLE_INTERRUPTS;

    for (;;) //for(;;)（开头）
    {
        // 取相应引脚的输入状态
        volatile uint8_t switch_0_status = gpio_get(SWITCH_0);
        volatile uint8_t switch_1_status = gpio_get(SWITCH_1);

        // volatile uint8_t flag = (portC_pin_1_state << 1) | portC_pin_1_state;
        volatile uint8_t flag = (switch_0_status << 1) | switch_1_status;
        printf("\nflag: %d", flag);

        switch (flag)
        {
        case 0:
            light_flash(LIGHT_RED, 0);
            break;
        case 1:
            light_flash(LIGHT_RED, 1000000);
            break;
        case 2:
            light_flash(LIGHT_RED, 5000000);
            break;
        case 3:
            light_flash(LIGHT_RED, 10000000);
            break;
        default:
            light_flash(LIGHT_RED, 0);
            printf("default");
            break;
        }
    } //for(;;)结尾
}

void light_flash(uint16_t port_pin, uint32_t delay_time)
{
    for (;;)
    {
        if (delay_time-- > 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    gpio_reverse(port_pin);
}