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

    uint8_t data[11] = "QIUYEYIJIAN";
    gI2CSendFlag = 0;
    i2c_m0_addr = 0x74;
    i2c_m1_addr = 0x73;


    //（1.2）【不变】关总中断
    DISABLE_INTERRUPTS;
    wdog_stop();

    // 初始化发送串口和接收串口
    suart_init(SUART_TX, SUART_SEND);
    suart_init(SUART_RX, SUART_RECEVIE);

    //I2C_M0初始化，I2C_M0:模块号 1:主机 0x74:地址  100:波特率(单位:KBps)
    i2c_init(I2C_M0, 0, i2c_m0_addr, 100);
    //I2C_M1初始化，I2C_M1:模块号 0:从机 0x73:地址  100:波特率(单位:KBps)
    i2c_init(I2C_M1, 0, i2c_m1_addr, 100);

    // 使能串口接收中断
    suart_enable_receive_interupt(SUART_RX);
    // 禁止i2c接收中断
    i2c_enable_re_int(I2C_M0);
    i2c_enable_re_int(I2C_M1);


    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;
    //（2）======主循环部分（结尾）========================================
    for (;;) {
        delay_ms(1000);

        if (gI2CSendFlag == 1) {
            i2c_send(I2C_M0, i2c_m1_addr, gRecvBuff, strlen(gRecvBuff));
            gI2CSendFlag = 0;
            printf("1111\n");

        }
        else if (gI2CSendFlag == 2) {
            i2c_send(I2C_M1, i2c_m0_addr, gRecvBuff, strlen(gRecvBuff));
            gI2CSendFlag = 0;
            printf("2222\n");
        }


        printf("Hello, World!\n");
    }
}

// 延时函数
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (6000 * ms); i++) {
        __asm("nop");
    }
}



