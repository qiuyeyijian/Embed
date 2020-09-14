
#include "lcd.h"

uint32_t mypow(uint8_t m,uint8_t n);
void LCD_WR_DATA8(uint8_t da);   //发送数据-8位参数
void LCD_WR_DATA(uint16_t da);
void LCD_WR_REG(uint8_t da);
void LCD_WR_REG_DATA(uint8_t reg,uint16_t da);
void Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Dly_ms(uint32_t ms);


//=====================================================================
//函数返回：无
//参数说明：无
//功能概要：LCD初始化
//修改记录：王宜怀，2020年9月1日，消除冗余语句
//=====================================================================
void LCD_Init(void)     //LCD初始化
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
 	////存储器访问控制，正向0x00/0x08,反向0xD0/0xD8
	LCD_WR_REG(0x36);    
	//0x00：ST7789V（新LCD）正常，ILI9341V红蓝颠倒
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xB6);    //显示功能控制
	LCD_WR_DATA8(0x0A);

	LCD_WR_DATA8(0xA2);

	LCD_WR_REG(0x3a);     //定义RGB图像数据格式
	LCD_WR_DATA8(0x05);   //16位/像素

	LCD_WR_REG(0x11);     //退出睡眠模式
	LCD_Dly_ms(800);        
	LCD_Clear(WHITE);
	LCD_WR_REG(0x29);
}

//=====================================================================
//函数返回：无
//参数说明：background:背景颜色（使用本文件后画笔颜色宏定义）
//功能概要：LCD清屏
//=====================================================================
void LCD_Clear(uint16_t background)  //LCD清屏
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
//函数返回：无
//参数说明：x,y:点坐标(240*320),
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个点
//=====================================================================
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)  //画点
{
	Address_set(x,y,x,y);//设置光标位置
	LCD_WR_DATA(color);
}

//=====================================================================
//函数返回：无
//参数说明：x,y:点坐标(240*320)
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个大点,大点的范围实际是一个以（x-1,y-1）为左
//         上角，（x+1,y+1）为右下角的小矩形，包含9个点（3*3）的大点。
//=====================================================================
void LCD_DrawPoint_big(uint16_t x,uint16_t y,uint16_t color) //画大点
{
	LCD_DrawSurface(x-1,y-1,x+1,y+1,color);
}

//=====================================================================
//函数返回：无
//参数说明：x,y:中心点坐标(240*320)
//         r:圆的半径
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置画一个指定大小的圆
//=====================================================================
void LCD_DrawCircle(uint16_t x,uint16_t y,uint8_t r,uint16_t color) 
{
	int a,b;
	int di;
	a=0;
	b=r;
	di=3-(r<<1);                      //判断下个点位置的标志
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
		//使用Bresenham算法画圆
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
//函数返回：无
//参数说明：x1,y1:左上角点坐标(240*320),x2,y2:右下角点坐标(240*320),
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定左上角与右下角画一个指定线条颜色的矩形
//=====================================================================
void LCD_DrawRectangle(uint16_t x1, uint16_t y1,
     uint16_t x2, uint16_t y2,uint16_t color)    //画矩形
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//=====================================================================
//函数返回：无
//参数说明：x,y:起点坐标(240*320)
//         fontcolor:字体颜色
//         groundbackcolor:背景颜色
//         p:字库起始地址
//功能概要：在指定位置显示一个汉字(16*16大小),汉字需要自行从字库提取
//=====================================================================
void LCD_ShowCharactor(uint16_t  x,uint16_t y,uint16_t fontcolor,
	 uint16_t groundbackcolor,const uint8_t *p)    //显示16*16汉字
{
	uint8_t i,j;
	const uint8_t *temp=p;
    Address_set(x,y,x+15,y+15); //设置区域
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
//函数返回：无
//参数说明：x,y:起点坐标(240*320)
//         fontcolor:字体颜色
//         p:字符串起始地址（可以包含中文，但应包含在汉字字模中）
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定位置显示字符串，可指定显示画笔颜色和背景颜色。
//          中文大小为（16*16）,英文大小为（8*16）
//=====================================================================
void LCD_ShowString(uint16_t  x,uint16_t y,uint16_t fontcolor,
	 uint16_t groundbackcolor,const uint8_t *p)
{
	uint8_t i,j,k;
	const uint8_t *temp;
	k = 0;
	while(*p != 0)
	{
		__asm("cpsid i");     // 关总中断
		if(*p>=0x20 && *p<=0x7e)
		{
			temp=nAsciiDot;
		    Address_set(x+8*k,y,x+7+8*k,y+15); //设置区域
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
		__asm("cpsie i");     // 开总中断
	}
}

//=====================================================================
//函数返回：无
//参数说明：x,y:左上坐标(240*320)
//         x1,y1:右下角坐标(240*320)
//功能概要：在指定位置显示一个凹凸按钮
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
//函数返回：无
//参数说明：x1,y1:起点坐标(240*320),x2,y2:终点坐标(240*320),
//       color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：在指定两点间画一条指定颜色的直线
//=====================================================================
void LCD_DrawLine(uint16_t x1,uint16_t y1,
	 uint16_t x2, uint16_t y2,uint16_t color)    //画线
{
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;                   //设置单步方向
	else if(delta_x==0)incx=0;            //垂直线
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;     	   //水平线
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )           //画线输出
	{
		LCD_DrawPoint(uRow,uCol,color);  //画点
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
//函数返回：无
//参数说明：x1,y1:起点坐标(240*320)
//         x2,y2:终点坐标(240*320)
//         color:颜色（使用本文件后画笔颜色宏定义）
//功能概要：指定颜色填充矩形区域
//=====================================================================
void LCD_DrawSurface(uint16_t x1,uint16_t y1,
     uint16_t x2,uint16_t y2,uint16_t color)    //填充矩形区域
{
	uint16_t i,j;
	Address_set(x1,y1,x2,y2);                 //设置光标位置
	for(i=y1;i<=y2;i++)
	{
		for(j=x1;j<=x2;j++)
		{
		    LCD_WR_DATA(color);               //画点
		}
	}
}

//=====================================================================
//内部函数
//=====================================================================

//=====================================================================
//函数返回：m^n
//参数说明：m底数,n指数
//功能概要：计算m^n
//=====================================================================
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//=====================================================================
//函数返回：无
//参数说明：da：传入一个字节的数据
//功能概要：发送数据-8位参数
//=====================================================================
void LCD_WR_DATA8(uint8_t da)    //发送数据-8位参数
{
	gpio_set(LCD_DC,1);
	gpio_init(LCD_CS,1,0);
	__asm("nop"); 
	spi_send1(0,da);
	gpio_init(LCD_CS,1,1);
}
//=====================================================================
//函数返回：无
//参数说明：da：传入一个字节的数据
//功能概要：发送数据-8位参数
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
	LCD_WR_REG(0x2a);      //写行地址命令
	LCD_WR_DATA8(x1>>8);   //写起始行地址
	LCD_WR_DATA8(x1);
	LCD_WR_DATA8(x2>>8);   //写结束行地址
	LCD_WR_DATA8(x2);

	LCD_WR_REG(0x2b);     //写列地址命令
	LCD_WR_DATA8(y1>>8);  //写列起始地址
	LCD_WR_DATA8(y1);
	LCD_WR_DATA8(y2>>8);  //写结束列地址
	LCD_WR_DATA8(y2);

	LCD_WR_REG(0x2C);     //写存储器命令
}
//======================================================================
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void LCD_Dly_ms(uint32_t ms)
{
   for (uint32_t ys=0;ys<(6000*ms);ys++) __asm("nop");
}
