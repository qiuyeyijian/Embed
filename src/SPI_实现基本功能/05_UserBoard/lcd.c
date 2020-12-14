
#include "lcd.h"

uint32_t mypow(uint8_t m,uint8_t n);
void LCD_WR_DATA8(uint8_t da);   //��������-8λ����
void LCD_WR_DATA(uint16_t da);
void LCD_WR_REG(uint8_t da);
void LCD_WR_REG_DATA(uint8_t reg,uint16_t da);
void Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Dly_ms(uint32_t ms);


//=====================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��LCD��ʼ��
//�޸ļ�¼�����˻���2020��9��1�գ������������
//=====================================================================
void LCD_Init(void)     //LCD��ʼ��
{

	spi_init(LCD_SPI,1,20000,0,0);
	gpio_init(LCD_RS,1,0);
	gpio_init(LCD_DC,1,0);
	gpio_init(LCD_CS,1,1);


	LCD_WR_REG_DATA(0,0);

	gpio_set(LCD_RS,0);
	LCD_Dly_ms(100);
	gpio_set(LCD_RS,1);
	LCD_Dly_ms(100);
 	////�洢�����ʿ��ƣ�����0x00/0x08,����0xD0/0xD8
	LCD_WR_REG(0x36);    
	//0x00��ST7789V����LCD��������ILI9341V�����ߵ�
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xB6);    //��ʾ���ܿ���
	LCD_WR_DATA8(0x0A);

	LCD_WR_DATA8(0xA2);

	LCD_WR_REG(0x3a);     //����RGBͼ�����ݸ�ʽ
	LCD_WR_DATA8(0x05);   //16λ/����

	LCD_WR_REG(0x11);     //�˳�˯��ģʽ
	LCD_Dly_ms(800);        
	LCD_Clear(WHITE);
	LCD_WR_REG(0x29);
}

//=====================================================================
//�������أ���
//����˵����background:������ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ��LCD����
//=====================================================================
void LCD_Clear(uint16_t background)  //LCD����
{
	uint16_t i,j;
	Address_set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	{
	  	for (j=0;j<LCD_H;j++)
	  	{
	  		LCD_WR_DATA(background);
	  	}
    }
}

//=====================================================================
//�������أ���
//����˵����x,y:������(240*320),
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ����
//=====================================================================
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)  //����
{
	Address_set(x,y,x,y);//���ù��λ��
	LCD_WR_DATA(color);
}

//=====================================================================
//�������أ���
//����˵����x,y:������(240*320)
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ�����,���ķ�Χʵ����һ���ԣ�x-1,y-1��Ϊ��
//         �Ͻǣ���x+1,y+1��Ϊ���½ǵ�С���Σ�����9���㣨3*3���Ĵ�㡣
//=====================================================================
void LCD_DrawPoint_big(uint16_t x,uint16_t y,uint16_t color) //�����
{
	LCD_DrawSurface(x-1,y-1,x+1,y+1,color);
}

//=====================================================================
//�������أ���
//����˵����x,y:���ĵ�����(240*320)
//         r:Բ�İ뾶
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ�û�һ��ָ����С��Բ
//=====================================================================
void LCD_DrawCircle(uint16_t x,uint16_t y,uint8_t r,uint16_t color) 
{
	int a,b;
	int di;
	a=0;
	b=r;
	di=3-(r<<1);                      //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x-b,y-a,color);             //3
		LCD_DrawPoint(x+b,y-a,color);             //0
		LCD_DrawPoint(x-a,y+b,color);             //1
		LCD_DrawPoint(x-b,y-a,color);             //7
		LCD_DrawPoint(x-a,y-b,color);             //2
		LCD_DrawPoint(x+b,y+a,color);             //4
		LCD_DrawPoint(x+a,y-b,color);             //5
		LCD_DrawPoint(x+a,y+b,color);             //6
		LCD_DrawPoint(x-b,y+a,color);
		a++;
		//ʹ��Bresenham�㷨��Բ
		if(di<0)
		{
			di +=4*a+6;
		}
		else
		{
			di+=10+4*(a-b);
			b--;
		}
		LCD_DrawPoint(x+a,y+b,color);
	}
}

//=====================================================================
//�������أ���
//����˵����x1,y1:���Ͻǵ�����(240*320),x2,y2:���½ǵ�����(240*320),
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ�����Ͻ������½ǻ�һ��ָ��������ɫ�ľ���
//=====================================================================
void LCD_DrawRectangle(uint16_t x1, uint16_t y1,
     uint16_t x2, uint16_t y2,uint16_t color)    //������
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//=====================================================================
//�������أ���
//����˵����x,y:�������(240*320)
//         fontcolor:������ɫ
//         groundbackcolor:������ɫ
//         p:�ֿ���ʼ��ַ
//���ܸ�Ҫ����ָ��λ����ʾһ������(16*16��С),������Ҫ���д��ֿ���ȡ
//=====================================================================
void LCD_ShowCharactor(uint16_t  x,uint16_t y,uint16_t fontcolor,
	 uint16_t groundbackcolor,const uint8_t *p)    //��ʾ16*16����
{
	uint8_t i,j;
	const uint8_t *temp=p;
    Address_set(x,y,x+15,y+15); //��������
	for(j=0;j<32;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)
				LCD_WR_DATA(fontcolor);
			else
				LCD_WR_DATA(groundbackcolor);
		}
		temp++;
	 }
}

//=====================================================================
//�������أ���
//����˵����x,y:�������(240*320)
//         fontcolor:������ɫ
//         p:�ַ�����ʼ��ַ�����԰������ģ���Ӧ�����ں�����ģ�У�
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ��λ����ʾ�ַ�������ָ����ʾ������ɫ�ͱ�����ɫ��
//          ���Ĵ�СΪ��16*16��,Ӣ�Ĵ�СΪ��8*16��
//=====================================================================
void LCD_ShowString(uint16_t  x,uint16_t y,uint16_t fontcolor,
	 uint16_t groundbackcolor,const uint8_t *p)
{
	uint8_t i,j,k;
	const uint8_t *temp;
	k = 0;
	while(*p != 0)
	{
		__asm("cpsid i");     // �����ж�
		if(*p>=0x20 && *p<=0x7e)
		{
			temp=nAsciiDot;
		    Address_set(x+8*k,y,x+7+8*k,y+15); //��������
			temp+=(*p-0x20)*16;
			for(j=0;j<16;j++)
			{
				for(i=0;i<8;i++)
				{
				 	if((*temp&(1<<i))!=0)
					{
						LCD_WR_DATA(fontcolor);
					}
					else
					{
						LCD_WR_DATA(groundbackcolor);
					}
				}
				temp++;
			 }
			k++;
		}
		else if(*p > 0x7e)
		{
             for(i=0;i<ChineseCharacterLength;i++)
             {
            	 uint8_t a = *p;
            	 uint8_t b = *(p+1);
            	 uint8_t c = GB_16[i].Index[0];
            	 uint8_t d = GB_16[i].Index[1];
                 if((a == c) && (b == d))
                 {
                	 LCD_ShowCharactor(x+8*k,y,fontcolor,
                	     groundbackcolor,GB_16[i].Msk);
                	 break;
                 }
             }
             k+=1;
		}
		p++;
		__asm("cpsie i");     // �����ж�
	}
}

//=====================================================================
//�������أ���
//����˵����x,y:��������(240*320)
//         x1,y1:���½�����(240*320)
//���ܸ�Ҫ����ָ��λ����ʾһ����͹��ť
//=====================================================================
void LCD_aotu(uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint8_t s)
{
   uint8_t i,n;
   uint16_t color;
   color = GRAY;
   LCD_DrawSurface(x,y,x1,y1,color);
   n=2;
   color = BLACK;
   LCD_DrawLine(x,y,x1,y,color);
   LCD_DrawLine(x,y,x,y1,color);
   color = BLACK;
   LCD_DrawLine(x1,y,x1,y1,color);
   LCD_DrawLine(x,y1,x1,y1,color);
   for(i=1;i<n;i++)
   {
   		color=(s==0?BLACK:WHITE);  
  	    LCD_DrawLine(x+i,y+i,x1-i,y+i,color);
        LCD_DrawLine(x+i,y+i,x+i,y1-i,color);
    	color=(s==0?WHITE:BLACK);
    	LCD_DrawLine(x1-i,y+i,x1-i,y1-i,color);
    	LCD_DrawLine(x+i,y1-i,x1-i,y1-i,color);
   };
};

//=====================================================================
//�������أ���
//����˵����x1,y1:�������(240*320),x2,y2:�յ�����(240*320),
//       color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ����ָ������仭һ��ָ����ɫ��ֱ��
//=====================================================================
void LCD_DrawLine(uint16_t x1,uint16_t y1,
	 uint16_t x2, uint16_t y2,uint16_t color)    //����
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //������������
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;                   //���õ�������
	else if(delta_x==0)incx=0;            //��ֱ��
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;     	   //ˮƽ��
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )           //�������
	{
		LCD_DrawPoint(uRow,uCol,color);  //����
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

//=====================================================================
//�������أ���
//����˵����x1,y1:�������(240*320)
//         x2,y2:�յ�����(240*320)
//         color:��ɫ��ʹ�ñ��ļ��󻭱���ɫ�궨�壩
//���ܸ�Ҫ��ָ����ɫ����������
//=====================================================================
void LCD_DrawSurface(uint16_t x1,uint16_t y1,
     uint16_t x2,uint16_t y2,uint16_t color)    //����������
{
	uint16_t i,j;
	Address_set(x1,y1,x2,y2);                 //���ù��λ��
	for(i=y1;i<=y2;i++)
	{
		for(j=x1;j<=x2;j++)
		{
		    LCD_WR_DATA(color);               //����
		}
	}
}

//=====================================================================
//�ڲ�����
//=====================================================================

//=====================================================================
//�������أ�m^n
//����˵����m����,nָ��
//���ܸ�Ҫ������m^n
//=====================================================================
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//=====================================================================
//�������أ���
//����˵����da������һ���ֽڵ�����
//���ܸ�Ҫ����������-8λ����
//=====================================================================
void LCD_WR_DATA8(uint8_t da)    //��������-8λ����
{
	gpio_set(LCD_DC,1);
	gpio_init(LCD_CS,1,0);
	__asm("nop"); 
	spi_send1(0,da);
	gpio_init(LCD_CS,1,1);
}
//=====================================================================
//�������أ���
//����˵����da������һ���ֽڵ�����
//���ܸ�Ҫ����������-8λ����
//=====================================================================
 void LCD_WR_DATA(uint16_t da)
{
	gpio_set(LCD_DC,1);
	gpio_init(LCD_CS,1,0);
	__asm("nop"); 
    spi_send1(0,da>>8);
    __asm("nop"); 
    spi_send1(0,da);
    gpio_init(LCD_CS,1,1);
}

void LCD_WR_REG(uint8_t da)
{
	gpio_set(LCD_DC,0);
	gpio_init(LCD_CS,1,0);
	__asm("nop");  
	spi_send1(0,da);
	gpio_init(LCD_CS,1,1);
}


 void LCD_WR_REG_DATA(uint8_t reg,uint16_t da)
{
    LCD_WR_REG(reg);
	LCD_WR_DATA(da);
}
void Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_WR_REG(0x2a);      //д�е�ַ����
	LCD_WR_DATA8(x1>>8);   //д��ʼ�е�ַ
	LCD_WR_DATA8(x1);
	LCD_WR_DATA8(x2>>8);   //д�����е�ַ
	LCD_WR_DATA8(x2);

	LCD_WR_REG(0x2b);     //д�е�ַ����
	LCD_WR_DATA8(y1>>8);  //д����ʼ��ַ
	LCD_WR_DATA8(y1);
	LCD_WR_DATA8(y2>>8);  //д�����е�ַ
	LCD_WR_DATA8(y2);

	LCD_WR_REG(0x2C);     //д�洢������
}
//======================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void LCD_Dly_ms(uint32_t ms)
{
   for (uint32_t ys=0;ys<(6000*ms);ys++) __asm("nop");
}
