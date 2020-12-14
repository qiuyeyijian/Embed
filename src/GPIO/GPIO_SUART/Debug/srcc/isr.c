//=====================================================================
//文件名称：isr.c（中断处理程序源文件）
//框架提供：SD-ARM（sumcu.suda.edu.cn）
//版本更新：20170801-20191020
//功能描述：提供中断处理程序编程框架
//=====================================================================
#include "includes.h"

void SUART_PortC_PortD_Handler(void) {

    uint32_t len = suart_receive(SUART_RX, gReceiveBuff);
    printf("gLenth: %d\n", len);

    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[0] >> i & 1);
    // }
    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[1] >> i & 1);
    // }
    // printf("=====================\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     printf("bit: %d\n", gReceiveBuff[2] >> i & 1);
    // }
    // printf("=====================\n");



    // for (uint32_t i = 0; i < len; i++) {
    //     printf("%c", gReceiveBuff[i]);
    // }

    suart_send(SUART_TX, gReceiveBuff, len);

}