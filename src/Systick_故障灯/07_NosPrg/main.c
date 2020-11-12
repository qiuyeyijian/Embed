//====================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：2017.08, 2020.06
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

    //（1.2）【不变】关总中断
    DISABLE_INTERRUPTS;

    //（1.4）给全局变量赋初值
    //"时分秒"缓存初始化(00:00:00)
    gTime[0] = 0;       //时
    gTime[1] = 0;	  	//分
    gTime[2] = 0;	  	//秒

    gFlag = 0;
    gStatus = 0;
    //（1.5）用户外设模块初始化
    gpio_init(LIGHT_BLUE, GPIO_OUTPUT, LIGHT_OFF);    //初始化蓝灯
    gpio_init(s0, GPIO_INPUT, 1);
    gpio_init(s1, GPIO_INPUT, 1);

    gpio_pull(s0, 0);
    gpio_pull(s1, 0);

    uart_init(UART_User, 115200);
    systick_init(10);      //设置systick为10ms中断
    //（1.6）使能模块中断
    uart_enable_re_int(UART_User);
    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;

}
