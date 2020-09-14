//======================================================================
//文件名称：ws2812.c
//功能概要：ws2812彩灯构件源文件
//制作单位：苏大arm技术中心(sumcu.suda.edu.cn)
//更新记录：20190309 ZYL
//======================================================================

#include "ws2812.h"    //包含ws2812头文件

//各端口基地址放入常数数据组
const PORT_MemMapPtr PORT_ARRs[] = {PORTA_BASE_PTR,PORTB_BASE_PTR,
        				PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//GPIO口基地址
const GPIO_MemMapPtr GPIO_ARRs[] = {PTA_BASE_PTR,PTB_BASE_PTR,
        				PTC_BASE_PTR,PTD_BASE_PTR,PTE_BASE_PTR};


//====================内部函数声明=======================================
void WS_Reset(uint16_t port_pin);    //重启，使改变生效。
void WS2812_Dly_ms(uint32_t ms);
//====================内部函数声明=======================================


//====================接口函数声明=======================================
//======================================================================
//函数名称：ws_Init
//函数参数：port_pin：控制ws2812的GPIO
//函数返回：无
//功能概要：初始化ws2812的gpio口
//======================================================================
void WS_Init(uint16_t port_pin)
{
	gpio_init(port_pin,1,0);
	WS2812_Dly_ms(10);
}

//======================================================================
//函数名称：ws_sendOnePix
//函数参数：port_pin：控制ws2812的GPIO;ptr：代表灯珠颜色的字节数组;num:灯珠数量
//函数返回：无
//功能概要：设置ws2812灯珠的颜色
//======================================================================
void WS_SendOnePix(uint16_t port_pin,uint8_t *ptr,uint8_t num)
{
	uint8_t i,j,temp;
	//局部变量声明
	PORT_Type *port_ptr;                       //声明port_ptr为PORT结构体类型指针
	GPIO_Type *gpio_ptr;                       //声明gpio_ptr为GPIO结构体类型指针
	uint8_t port,pin;                           //声明端口port、引脚pin变量
	//解析出端口与引脚分别赋给port,pin
	port=(port_pin>>8);
	pin=port_pin;
	//根据port，给局部变量port_ptr、gpio_ptr赋值（获得两个基地址）
	port_ptr = PORT_ARRs[port];                 //获得PORT模块相应口基地址
	gpio_ptr = GPIO_ARRs[port];                 //获得GPIO模块相应口基地址
	WS_Reset(port_pin);
	for(j=0;j<num*3;j++)
	{
		temp=ptr[j];
		for(i=0;i<8;i++)
		{
			if(temp&0x80)    //从高位开始发送
			{
				BSET(pin,GPIO_PDOR_REG(gpio_ptr));    //发送“1”码  26条
				for(volatile uint8_t ys=0;ys<=10;ys++) __asm("nop");
				
				BCLR(pin,GPIO_PDOR_REG(gpio_ptr));    //12
			    for(volatile uint8_t ys=0;ys<=10;ys++) __asm("nop");
				
			}
			else             //发送“0”码
			{
				BSET(pin,GPIO_PDOR_REG(gpio_ptr));
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");

				BCLR(pin,GPIO_PDOR_REG(gpio_ptr));
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");
				 __asm("nop");

			}
			temp=(temp<<1);    //左移一位，发送下一字节
		}
	}
	WS_Reset(port_pin);
}
//======================================================================
//函数名称：ws2812_reset
//函数参数：port_pin：控制ws2812的GPIO
//函数返回：无
//功能概要：重启ws2812，使改变生效。
//======================================================================
void WS_Reset(uint16_t port_pin)
{
	gpio_set(port_pin, 0);
	WS2812_Dly_ms(1);
}


//======================================================================
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void WS2812_Dly_ms(uint32_t ms)
{
   for (uint32_t ys=0;ys<(6000*ms);ys++) __asm("nop");
}