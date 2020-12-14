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
    wdog_stop();

    // 初始化发送串口和接收串口
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);
    LCD_Init();        //初始化LCD
    LCD_ShowString(6, 300, BLUE, GRAY, "Hello, world");
    LCD_ShowString(72, 32, BLUE, GRAY, "张龙威");
    LCD_DrawPoint_big(30, 30, RED);

    LCD_DrawLine(50, 50, 50, 150, RED);

    LCD_DrawRectangle(60, 60, 120, 120, YELLOW);

    LCD_DrawSurface(70, 70, 140, 140, BLUE);

    LCD_ShowString(120, 240, BLUE, GRAY, "秋叶依剑");

    // 使能串口接收中断
    suart_enable_receive_interupt(SUART_RX);

    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;
    //（2）======主循环部分（结尾）========================================

    for (;;) {

    }
}



