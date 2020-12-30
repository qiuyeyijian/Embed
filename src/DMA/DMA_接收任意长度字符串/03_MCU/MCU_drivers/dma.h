//=====================================================================
//文件名称：dma.h
//功能概要：dma底层驱动构件头文件
//制作单位：南通大学 计181张龙威
//版    本：  2020-12-31 V2.0
//适用芯片：KL36
//=====================================================================

#ifndef _DMA_H_
#define _DMA_H_
#include "mcu.h"  //包含公共要素头文件

// DMA模式
#define DMA_MODE_NORMAl 0    //普通模式
#define DMA_MODE_PERIODIC 1  //周期模式
// DMA传输宽度
#define DMA_TRANS_WIDTH_BIT32 (0x00)  // DMA传输宽度为32位
#define DMA_TRANS_WIDTH_BIT8 (0x01)   // DMA传输宽度为8位
#define DMA_TRANS_WIDTH_BIT16 (0x02)  // DMA传输宽度为16位
// DMA源地址模数大小
#define DMA_MODULE_DISABLE (0)  //禁用模块
#define DMA_MODULO_BYTE_16 (1)
#define DMA_MODULO_BYTE_32 (2)
#define DMA_MODULO_BYTE_64 (3)
#define DMA_MODULO_BYTE_128 (4)
#define DMA_MODULO_BYTE_256 (5)
#define DMA_MODULO_BYTE_512 (6)
#define DMA_MODULO_BYTE_1K (7)
#define DMA_MODULO_BYTE_2K (8)
#define DMA_MODULO_BYTE_4K (9)
#define DMA_MODULO_BYTE_8K (10)
#define DMA_MODULO_BYTE_16K (11)
#define DMA_MODULO_BYTE_32K (12)
#define DMA_MODULO_BYTE_64K (13)
#define DMA_MODULO_BYTE_128K (14)
#define DMA_MODULO_BYTE_256K (15)
//源地址偏移量开关
#define DMA_SRC_ADDR_OFFSET_ON (1)
#define DMA_SRC_ADDR_OFFSET_OFF (0)
//目标地址偏移量开关
#define DMA_DST_ADDR_OFFSET_ON (1)
#define DMA_DST_ADDR_OFFSET_OFF (0)
// 定义字节数计数寄存器最大值
#define DMA_DSR_BCR_MAXLEN (65535)
// DMA请求源
typedef enum DMA_sources {
  Channel_Disabled = 0,
  DMA_UART0_Rx = 2,
  DMA_UART0_Tx = 3,
  DMA_UART1_Rx = 4,
  DMA_UART1_Tx = 5,
  DMA_UART2_Rx = 6,
  DMA_UART2_Tx = 7,
  /*        I2S            */
  DMA_I2S0_Rx = 14,
  DMA_I2S0_Tx = 15,
  /*        SPI            */
  DMA_SPI0_Rx = 16,
  DMA_SPI0_Tx = 17,
  DMA_SPI1_Rx = 18,
  DMA_SPI1_Tx = 19,
  /*        I2C            */
  DMA_I2C0 = 22,
  DMA_I2C1 = 23,
  /*        TPM            */
  DMA_TPM0_CH0 = 24,
  DMA_TPM0_CH1 = 25,
  DMA_TPM0_CH2 = 26,
  DMA_TPM0_CH3 = 27,
  DMA_TPM0_CH4 = 28,
  DMA_TPM0_CH5 = 29,
  DMA_TPM1_CH0 = 32,
  DMA_TPM1_CH1 = 33,
  DMA_TPM2_CH0 = 34,
  DMA_TPM2_CH1 = 35,
  /*     ADC/DAC/CMP    */
  DMA_ADC0 = 40,
  DMA_CMP0 = 42,
  DMA_DAC0 = 45,
  DMA_Port_A = 49,
  DMA_Port_C = 51,
  DMA_Port_D = 52,
  DMA_TPM0_OVERFLOW = 54,
  DMA_TPM1_OVERFLOW = 55,
  DMA_TPM2_OVERFLOW = 56,
  DMA_TSI = 57,
  DMA_Always_EN1 = 60,
  DMA_Always_EN2 = 61,
  DMA_Always_EN3 = 62,
  DMA_Always_EN4 = 63,
} DMA_sources;

//定义DMA通道状态
typedef enum {
  DMAChannelIdle,       //通道空闲
  DMAChannelStarting,   //通道正在启动
  DMAChannelExecuting,  //通道正在执行
  DMAChannelDone        //通道完成主循环
} DMAChannelState;

//==========================================================================
//函数名称: dma_uart_init
//函数返回: 无
//参数说明:  chSend: DMA发送通道
//         chRecv: DMA接收通道
//         buff: DMA在内存中的缓冲区
//功能概要: 初始化UART的DMA功能
//==========================================================================
void dma_uart_init(uint8_t chSend, uint8_t chRecv, uint32_t buff);

//==========================================================================
//函数名称: dma_uart_recv
//函数返回: 无
//参数说明:  chRecv: DMA接收通道
//         dstAddress: 目的地址。源地址固定为UART0的数据寄存器地址
//         len: 接收的数据长度
//功能概要:  使能一次DMA接收传输
//==========================================================================
void dma_uart_recv(uint8_t chRecv, uint32_t dstAddress, uint32_t len);

//==========================================================================
//函数名称: dma_uart_send
//函数返回: 无
//参数说明:  chSend: DMA发送通道
//         srcAddress: 源地址。目的地址固定为UART0的数据寄存器地址
//         len: 接收的数据长度
//功能概要:  使能一次DMA发送传输
//==========================================================================
void dma_uart_send(uint8_t chSend, uint32_t srcAddress, uint32_t len);

//==========================================================================
//函数名称: dma_get_recv_count
//函数返回: 无
//参数说明:  chRecv: DMA接收通道
//功能概要:  获取DMA接收通道接收数据个数
//==========================================================================
uint32_t dma_get_recv_count(uint8_t chRecv);

//==========================================================================
//函数名称: dma_enable_int
//函数返回: 无
//参数说明:  ch: DMA通道
//功能概要:  使能DMA通道中断
//==========================================================================
void dma_enable_int(uint8_t ch);

//==========================================================================
//函数名称: dma_clear
//函数返回: 无
//参数说明:  ch: DMA通道
//功能概要:  清通道中断与错误标志
//==========================================================================
void dma_clear(uint8_t ch);

#endif
