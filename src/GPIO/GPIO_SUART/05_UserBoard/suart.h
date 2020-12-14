#include "mcu.h"
#include "printf.h"
#include "gec.h"

// 端口起始地址
#define PORT_START_BASE ((uint32_t*)(0x40049000u))
#define SUART_START_BASE ((uint32_t*)(0x400FF000u))
// 每个端口基地址相差 100H
#define PORT_BASE(port) (PORT_START_BASE + (port*1024))
// 相应引脚的控制寄存器
#define PORT_PCR(port, pin) (*(PORT_BASE(port) + pin))
// 每个SUART口的基地址相差 40H
#define SUART_BASE(port) (SUART_START_BASE + (port*16))
// 每个SUART口相应的寄存器
#define SUART_PDOR(port) (*(SUART_BASE(port)+0))
#define SUART_PSOR(port) (*(SUART_BASE(port)+1))
#define SUART_PCOR(port) (*(SUART_BASE(port)+2))
#define SUART_PTOR(port) (*(SUART_BASE(port)+3))
#define SUART_PDIR(port) (*(SUART_BASE(port)+4))
#define SUART_PDDR(port) (*(SUART_BASE(port)+5))

// 端口号地址偏移量宏定义
#define PTA_NUM    (0<<8)
#define PTB_NUM    (1<<8)
#define PTC_NUM    (2<<8)
#define PTD_NUM    (3<<8)
#define PTE_NUM    (4<<8)

// 发送和接收方向定义
#define SUART_SEND (1)
#define SUART_RECEVIE (0)

//【变动】SUART可用模块定义
// MCU发送端口和引脚号
#define  SUART_TX   (PTC_NUM | 0)  // GEC_40
// MCU接收端口和引脚号
#define  SUART_RX   (PTC_NUM | 1)  // GEC_39

// 根据示波器调试出来，MCU做空操作22次，可模拟波特率115200，也就是位长大概8.69us
#define delayCount (22)     // 波特率 115200
// #define delayCount (300)        // 波特率 9600

// 模拟串口初始化
void suart_init(uint16_t port_pin, uint8_t dir);

// 设置串口状态
void suart_set(uint8_t port, uint8_t pin, uint8_t status);

// 获取串口状态 
uint8_t suart_get(uint8_t port, uint8_t pin);

// 发送字符
void suart_send(uint16_t port_pin, uint8_t* arr, uint32_t len);

// 接收字符
uint32_t suart_receive(uint16_t port_pin, uint8_t* arr);

// 接收一个字符

// 打开串口接收中断, 下降沿触发
void suart_enable_receive_interupt(uint16_t port_pin);