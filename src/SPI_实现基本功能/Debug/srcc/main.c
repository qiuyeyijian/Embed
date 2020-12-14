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
void delay_ms(uint32_t ms);
//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //（1.2）【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    uint8_t data[20] = "QIUYEYIJIAN";
    // 初始化SPI发送标标志
    gSendFlag = 0;

    // 初始化发送串口和接收串口
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);

    // 将SPI0初始化为主机，波特率为6000， 时钟极性0，相位0
    spi_init(SPI_SEND, 1, 6000, 0, 0);
    // 将SPI1初始化为主机，波特率为6000， 时钟极性0，相位0
    spi_init(SPI_RECEIVE, 0, 6000, 0, 0);



    // 使能串口接收中断
    suart_enable_receive_interupt(SUART_RX);
    // 使能SPI1模块中断
    spi_enable_re_int(SPI_RECEIVE);

    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;
    //（2）======主循环部分（结尾）========================================

    for (;;) {
        // delay_ms(1000);
        //SPI数据发送
        if (gSendFlag) {
            printf("主机字符串开始发送!\n");
            spi_sendN(SPI_SEND, strlen(gRecvBuff), gRecvBuff);      //发送数据
            printf("主机字符串发送完毕!\n");
            gSendFlag = 0;
        }
    }
}


void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (6000 * ms); i++) __asm("nop");
}



