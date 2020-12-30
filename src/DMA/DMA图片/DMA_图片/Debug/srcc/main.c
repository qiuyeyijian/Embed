//====================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：SD-Arm（sumcu.suda.edu.cn）
//版本更新：2017.08, 2020.06
//功能描述：见本工程的<01_Doc>文件夹下Readme.txt文件
//====================================================================

#define GLOBLE_VAR
#include "includes.h"  //包含总头文件

//----------------------------------------------------------------------
//声明使用到的内部函数
// main.c使用的内部函数声明处
void delay_ms(uint32_t ms);
//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void) {
  //（1）======启动部分（开头）==========================================
  //（1.1）声明main函数使用的局部变量

  //（1.2）【不变】关总中断
  DISABLE_INTERRUPTS;
  wdog_stop();

  //（1.3）给主函数使用的局部变量赋初值

  //（1.4）给全局变量赋初值
  gLen = 0;

  //（1.5）用户外设模块初始化
  // DMA通道0发送，DMA通道1接收
  dma_uart_init(DMA_SEND, DMA_RECV, (uint32_t)gRecvBuff);
  uart_init(UART_User, 115200);
  flash_init();

  //（1.6）使能模块中断
  uart_enable_idle_int(UART_User);  // 使能空闲线中断, 不要使能接收中断
  //（1.7）【不变】开总中断
  ENABLE_INTERRUPTS;

  //（1）======启动部分（结尾）==========================================

  //（2）======主循环部分（开头）=========================================
  for (;;) {
    if (gLen) {
      uint8_t cmd = gRecvBuff[0];
      uint16_t sect = gRecvBuff[1];

      switch (cmd) {
        case '1':  // 写文件数据
          flash_erase(sect);
          flash_write(sect, 0, 1024, gRecvBuff + 2);
          // dma_uart_send(DMA_SEND, (uint32_t)gRecvBuff, strlen(gRecvBuff));
          break;
        case '2':  // 读数据
          flash_read_logic(gRecvBuff, sect, 0, 1024);
          printf("读取长度：%d\n", strlen(gRecvBuff));
          // suart_send(SUART_TX, gRecvBuff, strlen(gRecvBuff));
          dma_uart_send(DMA_SEND, (uint32_t)gRecvBuff, strlen(gRecvBuff));
          // delay_ms(10);
          break;

        default:
          break;
      }
      // dma_uart_send(DMA_SEND, (uint32_t)gRecvBuff, gLen);
      gLen = 0;
      printf("\nDMA Read Success!\n");
    }

  }  // for(;;)结尾
  //（2）======主循环部分（结尾）========================================
}

//======以下为主函数调用的子函数存放处=====================================
void delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < (6000 * ms); i++) {
    __asm("nop");
  }
}
//====================================================================
/*
知识要素：
（1）main.c是一个模板，该文件所有代码均不涉及具体的硬件和环境，通过调用构件
实现对硬件的干预。
（2）本文件中标有【不变】的地方为系统保留，此类代码与具体项目无关，不宜删除。
（3）本文件中对宏GLOBLE_VAR进行了定义，所以在包含"includes.h"头文件时，会定
义全局变量，在其他文件中包含"includes.h"头文件时，
编译时会自动增加extern
*/
