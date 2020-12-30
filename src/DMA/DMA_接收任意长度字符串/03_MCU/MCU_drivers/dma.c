//=====================================================================
//文件名称：dma.h
//功能概要：dma底层驱动构件头文件
//制作单位：南通大学 计181张龙威
//版    本：  2020-12-31 V2.0
//适用芯片：KL36
//=====================================================================
#include "dma.h"

// DMA各通道中断请求号
static const IRQn_Type dma_irq_table[] = {
    DMA0_IRQn,
    DMA1_IRQn,
    DMA2_IRQn,
    DMA3_IRQn,
};

//==========================================================================
//函数名称: dma_uart_init
//函数返回: 无
//参数说明:  chSend: DMA发送通道
//         chRecv: DMA接收通道
//         buff: DMA在内存中的缓冲区
//功能概要: 初始化UART的DMA功能
//==========================================================================
void dma_uart_init(uint8_t chSend, uint8_t chRecv, uint32_t buff) {
  //使能UART0的DMA发送
  SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;  //启动串口0时钟
  UART0_C5 |= UART_C4_TDMAS_MASK;     //使能UART0的TDMA
  UART0_C5 |= UART_C4_RDMAS_MASK;     //使能UART0的RDMA
  // UART0_C2 |= UART_C2_TIE_MASK;       //使能UART0发送中断
  // UART0_C2 |= UART_C2_RIE_MASK;       //使能UART0接收中断
  // UART0_C2 &= ~UART_C2_TCIE_MASK;  //禁用UART0发送完成中断

  //开启DMA和DMAMUX模块时钟门
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;  // 开DMA MUX模块时钟
  SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;     // 开 DMA模块时钟
  // 初始化DMA通道配置寄存器，默认DMA通道禁止，普通触发
  DMAMUX0_CHCFG(chSend) = 0x00;
  DMAMUX0_CHCFG(chRecv) = 0x00;
  // 设置DMA通道触发请求源
  DMAMUX0_CHCFG(chSend) |= DMAMUX_CHCFG_SOURCE(DMA_UART0_Tx);
  DMAMUX0_CHCFG(chRecv) |= DMAMUX_CHCFG_SOURCE(DMA_UART0_Rx);

  // 清除DMA状态寄存器/字节数寄存器中的错误、中断和完成标志
  dma_clear(chSend);
  dma_clear(chRecv);

  // 初始化DMA控制寄存器
  // 默认不运行外设DMA请求，即忽略外设发起的DMA请求
  DMA_DCR(chSend) = 0x00;
  DMA_DCR(chRecv) = 0x00;

  //设置源地址和目标地址的模数大小和数据宽度以及其他相关标志
  DMA_DCR(chSend) |=
      (DMA_DCR_SSIZE(1)    //配置源地址数据长度为8位
       | DMA_DCR_DSIZE(1)  //目的地址数据长度为8位
       | DMA_DCR_SMOD(0)  // 禁止源地址取模，循环模式下需要开启
       | DMA_DCR_DMOD(0)  // 禁止目的地址取模，循环模式需要开启
       | DMA_DCR_D_REQ_MASK  // BCR到0时外设请求清除
       | DMA_DCR_SINC_MASK  // 源地址允许增加，每次传输后的目的地址增加一个字节
       | DMA_DCR_CS_MASK   //每次请求都会产生一次读写传输
       | DMA_DCR_ERQ_MASK  //允许外设请求
       //| DMA_DCR_EINT_MASK  // 传输完成中断使能
       | DMA_DCR_EADREQ_MASK  //允许异步请求

      );

  DMA_DCR(chRecv) |=
      (DMA_DCR_SSIZE(1)    //配置源地址数据长度为8位
       | DMA_DCR_DSIZE(1)  //目的地址数据长度为8位
       | DMA_DCR_SMOD(0)  // 禁止源地址取模，循环模式下需要开启
       | DMA_DCR_DMOD(0)  // 禁止目的地址取模，循环模式需要开启
       | DMA_DCR_D_REQ_MASK  // BCR到0时外设请求清除
       | DMA_DCR_DINC_MASK  // 源地址允许增加，每次传输后的目的地址增加一个字节
       | DMA_DCR_CS_MASK   //每次请求都会产生一次读写传输
       | DMA_DCR_ERQ_MASK  //允许外设请求
       //| DMA_DCR_EINT_MASK  // 传输完成中断使能
       | DMA_DCR_EADREQ_MASK  //允许异步请求

      );

  // 设置发送通道目的地址, 应该是UART0的数据寄存器地址
  // 发送通道的源地址和目的地址，从内存到外设UART0的数据寄存器
  DMA_SAR(chSend) = (uint32_t)buff;
  DMA_DAR(chSend) = (uint32_t)&UART0_D;
  // 接收通道源地址和目的地址，从外设UART0的数据寄存器到内存buff
  DMA_SAR(chRecv) = (uint32_t)&UART0_D;
  DMA_DAR(chRecv) = (uint32_t)buff;

  //发送通道字节数计数寄存器设置
  DMA_DSR_BCR(chSend) &= ~DMA_DSR_BCR_BCR_MASK;  // 一定要先清零
  DMA_DSR_BCR(chSend) = DMA_DSR_BCR_BCR(0);      // 初始化先不要发数据
  //接收通道字节数计数寄存器设置
  // 每接收一个字节，其数值减一，为0的时候自动清除外设的请求
  DMA_DSR_BCR(chRecv) &= ~DMA_DSR_BCR_BCR_MASK;  // 一定要先清零
  DMA_DSR_BCR(chRecv) = DMA_DSR_BCR_BCR(DMA_DSR_BCR_MAXLEN);

  // 使能DMA通道
  DMAMUX0_CHCFG(chSend) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMAMUX0_CHCFG(chRecv) |= (DMAMUX_CHCFG_ENBL_MASK);
}

//==========================================================================
//函数名称: dma_uart_recv
//函数返回: 无
//参数说明:  chRecv: DMA接收通道
//         dstAddress: 目的地址。源地址固定为UART0的数据寄存器地址
//         len: 接收的数据长度
//功能概要:  使能一次DMA接收传输
//==========================================================================
void dma_uart_recv(uint8_t chRecv, uint32_t dstAddress, uint32_t len) {
  //设置目标地址
  DMA_DAR(chRecv) = dstAddress;

  //清中断与错误标志
  dma_clear(chRecv);

  //暂时关闭该DMA通道，默认触发模式为正常模式
  DMA_DCR(chRecv) &= ~DMA_DCR_ERQ_MASK;
  DMAMUX0_CHCFG(chRecv) &= ~(DMAMUX_CHCFG_ENBL_MASK);

  //字节数计数寄存器设置
  DMA_DSR_BCR(chRecv) &= ~DMA_DSR_BCR_BCR_MASK;  // 一定要先清零
  DMA_DSR_BCR(chRecv) = DMA_DSR_BCR_BCR(len);

  // 使能该DMA通道
  DMAMUX0_CHCFG(chRecv) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMA_DCR(chRecv) |= DMA_DCR_ERQ_MASK;
}

//==========================================================================
//函数名称: dma_uart_send
//函数返回: 无
//参数说明:  chSend: DMA发送通道
//         srcAddress: 源地址。目的地址固定为UART0的数据寄存器地址
//         len: 接收的数据长度
//功能概要:  使能一次DMA发送传输
//==========================================================================
void dma_uart_send(uint8_t chSend, uint32_t srcAddress, uint32_t len) {
  //判断传输数目参数是否合法
  if ((len & 0xFFF00000) > 0) {
    len = 0xFFFFF;
  }

  //清中断与错误标志
  dma_clear(chSend);

  //暂时关闭该DMA通道，并设置该通道的请求源，默认触发模式为正常模式
  DMA_DCR(chSend) &= ~DMA_DCR_ERQ_MASK;
  DMAMUX0_CHCFG(chSend) &= ~(DMAMUX_CHCFG_ENBL_MASK);

  //设置目的地址, 应该是UART0的数据寄存器地址
  DMA_DAR(chSend) = (uint32_t)&UART0_D;

  //设置源地址
  DMA_SAR(chSend) = srcAddress;

  //字节数计数寄存器设置
  DMA_DSR_BCR(chSend) &= ~DMA_DSR_BCR_BCR_MASK;  // 一定要先清零
  DMA_DSR_BCR(chSend) = DMA_DSR_BCR_BCR(len);

  //使能该DMA通道
  DMAMUX0_CHCFG(chSend) |= (DMAMUX_CHCFG_ENBL_MASK);
  DMA_DCR(chSend) |= DMA_DCR_ERQ_MASK;
}

//==========================================================================
//函数名称: dma_get_recv_count
//函数返回: 无
//参数说明:  chRecv: DMA接收通道
//功能概要:  获取DMA接收通道接收数据个数
//==========================================================================
uint32_t dma_get_recv_count(uint8_t chRecv) {
  // 最大值减去剩余的值就是接收的数据个数
  return (DMA_DSR_BCR_MAXLEN -
          (uint32_t)(DMA_DSR_BCR(chRecv) & DMA_DSR_BCR_BCR_MASK));
}

//==========================================================================
//函数名称: dma_enable_int
//函数返回: 无
//参数说明:  ch: DMA通道
//功能概要:  使能DMA通道中断
//==========================================================================
void dma_enable_int(uint8_t ch) {
  DMA_DCR(ch) |= DMA_DCR_EINT_MASK;   // 开启通道中断使能
  NVIC_EnableIRQ(dma_irq_table[ch]);  //开中断控制器IRQ中断
}

//======================================================================
//函数名称：dma_clear
//参数说明：ch:
//函数返回：清通道中断与错误标志
//======================================================================
void dma_clear(uint8_t ch) {
  if ((DMA_DSR_BCR(ch) & DMA_DSR_BCR_DONE_MASK) == DMA_DSR_BCR_DONE_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_BES_MASK) == DMA_DSR_BCR_BES_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_BED_MASK) == DMA_DSR_BCR_BED_MASK ||
      (DMA_DSR_BCR(ch) & DMA_DSR_BCR_CE_MASK) == DMA_DSR_BCR_CE_MASK) {
    DMA_DSR_BCR(ch) |= DMA_DSR_BCR_DONE_MASK;  //清DMA传输完成标志, 写1清除
  }
}
