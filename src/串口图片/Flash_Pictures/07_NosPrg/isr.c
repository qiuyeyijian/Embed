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

    uint8_t cmd = gReceiveBuff[0];
    uint16_t sect = gReceiveBuff[1];

    uint8_t i = 0;

    switch (cmd)
    {
    case '1':   // 写文件数据
        flash_erase(sect);
        flash_write(sect, 0, 1024, gReceiveBuff + 2);
        break;
    case '2':   // 读数据
        flash_read_logic(gReceiveBuff, sect, 0, 1024);
        printf("读取长度：%d\n", strlen(gReceiveBuff));
        suart_send(SUART_TX, gReceiveBuff, strlen(gReceiveBuff));
        break;

    default:
        break;
    }

}