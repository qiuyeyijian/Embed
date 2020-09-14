//======================================================================
//�ļ����ƣ�ws2812.c
//���ܸ�Ҫ��ws2812�ʵƹ���Դ�ļ�
//������λ���մ�arm��������(sumcu.suda.edu.cn)
//���¼�¼��20190309 ZYL
//======================================================================

#include "ws2812.h"    //����ws2812ͷ�ļ�

//���˿ڻ���ַ���볣��������
const PORT_MemMapPtr PORT_ARRs[] = {PORTA_BASE_PTR,PORTB_BASE_PTR,
        				PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//GPIO�ڻ���ַ
const GPIO_MemMapPtr GPIO_ARRs[] = {PTA_BASE_PTR,PTB_BASE_PTR,
        				PTC_BASE_PTR,PTD_BASE_PTR,PTE_BASE_PTR};


//====================�ڲ���������=======================================
void WS_Reset(uint16_t port_pin);    //������ʹ�ı���Ч��
void WS2812_Dly_ms(uint32_t ms);
//====================�ڲ���������=======================================


//====================�ӿں�������=======================================
//======================================================================
//�������ƣ�ws_Init
//����������port_pin������ws2812��GPIO
//�������أ���
//���ܸ�Ҫ����ʼ��ws2812��gpio��
//======================================================================
void WS_Init(uint16_t port_pin)
{
	gpio_init(port_pin,1,0);
	WS2812_Dly_ms(10);
}

//======================================================================
//�������ƣ�ws_sendOnePix
//����������port_pin������ws2812��GPIO;ptr�����������ɫ���ֽ�����;num:��������
//�������أ���
//���ܸ�Ҫ������ws2812�������ɫ
//======================================================================
void WS_SendOnePix(uint16_t port_pin,uint8_t *ptr,uint8_t num)
{
	uint8_t i,j,temp;
	//�ֲ���������
	PORT_Type *port_ptr;                       //����port_ptrΪPORT�ṹ������ָ��
	GPIO_Type *gpio_ptr;                       //����gpio_ptrΪGPIO�ṹ������ָ��
	uint8_t port,pin;                           //�����˿�port������pin����
	//�������˿������ŷֱ𸳸�port,pin
	port=(port_pin>>8);
	pin=port_pin;
	//����port�����ֲ�����port_ptr��gpio_ptr��ֵ�������������ַ��
	port_ptr = PORT_ARRs[port];                 //���PORTģ����Ӧ�ڻ���ַ
	gpio_ptr = GPIO_ARRs[port];                 //���GPIOģ����Ӧ�ڻ���ַ
	WS_Reset(port_pin);
	for(j=0;j<num*3;j++)
	{
		temp=ptr[j];
		for(i=0;i<8;i++)
		{
			if(temp&0x80)    //�Ӹ�λ��ʼ����
			{
				BSET(pin,GPIO_PDOR_REG(gpio_ptr));    //���͡�1����  26��
				for(volatile uint8_t ys=0;ys<=10;ys++) __asm("nop");
				
				BCLR(pin,GPIO_PDOR_REG(gpio_ptr));    //12
			    for(volatile uint8_t ys=0;ys<=10;ys++) __asm("nop");
				
			}
			else             //���͡�0����
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
			temp=(temp<<1);    //����һλ��������һ�ֽ�
		}
	}
	WS_Reset(port_pin);
}
//======================================================================
//�������ƣ�ws2812_reset
//����������port_pin������ws2812��GPIO
//�������أ���
//���ܸ�Ҫ������ws2812��ʹ�ı���Ч��
//======================================================================
void WS_Reset(uint16_t port_pin)
{
	gpio_set(port_pin, 0);
	WS2812_Dly_ms(1);
}


//======================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void WS2812_Dly_ms(uint32_t ms)
{
   for (uint32_t ys=0;ys<(6000*ms);ys++) __asm("nop");
}