//=====================================================================
//文件名称：isr.c（中断处理程序源文件）
//框架提供：SD-ARM（sumcu.suda.edu.cn）
//版本更新：20170801-20191020
//功能描述：提供中断处理程序编程框架
//=====================================================================
#include "includes.h"

void SUART_PortC_PortD_Handler(void) {

    uint32_t len = suart_receive(SUART_RX, gRecvBuff);
    printf("gLenth: %d\n", len);
    gSendFlag = 1;

}

void SPI_Send_Handler(void) {
    uint8_t data[20];
    DISABLE_INTERRUPTS;     //关总中断
    //------------------------------------------------------------------
    //（在此处增加功能）
    data[0] = spi_receive1(SPI_SEND);    //接收1个字节的数据

    suart_send(SUART_TX, data, 1);
    //------------------------------------------------------------------
    ENABLE_INTERRUPTS;     //关总中断
}

void SPI_Receive_Handler(void) {
    DISABLE_INTERRUPTS;     //关总中断
    //------------------------------------------------------------------
    //（在此处增加功能）
    uint32_t len = spi_receiveN(SPI_RECEIVE, gRecvBuff);
    // 如果len的长度不为0,说明字符接收成功，长度为len
    if (len) {
        printf("%s\n", gRecvBuff);
        suart_send(SUART_TX, gRecvBuff, len);
    }

    //------------------------------------------------------------------
    ENABLE_INTERRUPTS;     //关总中断
}
