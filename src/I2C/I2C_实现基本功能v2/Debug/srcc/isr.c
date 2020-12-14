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

    gI2CSendFlag = gRecvBuff[0] - '0';
}

void I2C_M0_Handler(void) {
    printf("send\n");
    static uint8_t isMe = 0;
    DISABLE_INTERRUPTS;     //关总中断

    // 如果isMe = 0; 则进行判断是否是和自己通信,
    // 如果地址匹配，说明是和自己通信，令isMe = 1
    if (!isMe) {
        // 因为地址和数据方向位合并在一起了，所以判断的时候也要现将地址和方向合并再判断
        if ((i2c_m0_addr << 1 & 0xFE) == i2c_read(I2C_M0)) {
            isMe = 1;
        }
    }
    else {
        uint32_t len = i2c_recv(I2C_M0, gRecvBuff);
        if (len) {
            suart_send(SUART_TX, gRecvBuff, strlen(gRecvBuff));
            isMe = 0;
        }
    }

    // 清中断
    i2c_clear_re_int(I2C_M0);
    ENABLE_INTERRUPTS;      //开总中断
}

void I2C_M1_Handler(void) {
    printf("receive\n");
    static uint8_t isMe = 0;
    DISABLE_INTERRUPTS;     //关总中断

    // 如果isMe = 0; 则进行判断是否是和自己通信,
    // 如果地址匹配，说明是和自己通信，令isMe = 1
    if (isMe == 0) {
        // 因为地址和数据方向位合并在一起了，所以判断的时候也要现将地址和方向合并再判断
        if ((i2c_m1_addr << 1 & 0xFE) == i2c_read(I2C_M1)) {
            isMe = 1;
        }
    }
    else {
        uint32_t len = i2c_recv(I2C_M1, gRecvBuff);
        if (len) {
            suart_send(SUART_TX, gRecvBuff, strlen(gRecvBuff));
            // 当处理完毕接收的字符串后，令isMe = 0; 方便进行下一次通信判断
            isMe = 0;
        }

    }

    i2c_clear_re_int(I2C_M1);
    ENABLE_INTERRUPTS;      //开总中断
}