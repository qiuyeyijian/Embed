//=====================================================================
//文件名称：spi.c
//功能概要：spi底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：KL25、KL26、KL36
//=====================================================================
#include "spi.h"


//=====================================================================
//功能说明：SPI初始化
//函数参数：No：模块号，可用参数可参见gec.h文件
//       MSTR：SPI主从机选择，0选择为从机,1选择为主机。
//       BaudRate：波特率，可取12000、6000、4000、3000、1500、1000，
//       单位：bps
//       CPOL：CPOL=0：高有效SPI时钟（低无效）；
//       CPOL=1：低有效SPI时钟（高无效）
//       CPHA：CPHA=0相位为0； CPHA=1相位为1；
//函数返回：无
//=====================================================================
void spi_init(uint8_t No, uint8_t MSTR, uint16_t BaudRate, \
    uint8_t CPOL, uint8_t CPHA)
{
    uint8_t BaudRate_Mode;
    uint8_t BaudRate_High;
    uint8_t BaudRate_Low;
    //如果SPI号参数错误则强制选择 0号模块
    if (No<SPI_0 || No>SPI_1)   No = SPI_0;
    if (No == SPI_0)                         //初始化SPI0功能
    {
        BSET(SIM_SCGC4_SPI0_SHIFT, SIM_SCGC4);   //打开SPI0模块时钟。
        //引脚复用为SPI0功能
        PORTC_PCR4 = (0 | PORT_PCR_MUX(0x02));     //选择PTC4的SS功能
        PORTC_PCR5 = (0 | PORT_PCR_MUX(0x02));     //选择PTC5的SCK功能
        PORTC_PCR6 = (0 | PORT_PCR_MUX(0x02));     //选择PTC6的MOSI功能
        PORTC_PCR7 = (0 | PORT_PCR_MUX(0x02));     //选择PTC7的MIOS功能

        SPI0_C1 = 0x00;                           //SPI控制寄存器1清零
        BSET(SPI_C1_SPE_SHIFT, SPI0_C1);         //使能SPI模块

        //MSTR=1为主机模式；
        //MSTR=0为从机模式（因MSTR初始值为0，无需更改）
        (MSTR == 1) ? BSET(SPI_C1_MSTR_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_SPIE_SHIFT, SPI0_C1);

        //时钟极性配置，CPOL=0，平时时钟为高电平，反之CPOL=1，平时时钟为低电平
        (0 == CPOL) ? BCLR(SPI_C1_CPOL_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_CPOL_SHIFT, SPI0_C1);

        //时钟相位CPHA
        (0 == CPHA) ? BCLR(SPI_C1_CPHA_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_CPHA_SHIFT, SPI0_C1);

        //SSOE为1,MODFEN为1，配置本模块为自动SS输出
        BSET(SPI_C1_SSOE_SHIFT, SPI0_C1);
        //对SPI0的C1寄存器配置为主机模式、从机选择自动模式并使能SPI0模块。
        SPI0_C2 = 0x00;
        if (MSTR == 1)  //主机模式
            BSET(SPI_C2_MODFEN_SHIFT, SPI0_C2);
        SPI0_BR = 0x00U;//波特率寄存器清零
        //重新设置波特率
        BaudRate_High = 0;
        BaudRate_Low = 0;
        BaudRate_Mode = 24000 / BaudRate;
        if (BaudRate_Mode <= 8)
        {
            SPI0_BR = (BaudRate_Mode - 1) << 4;
        }
        else
        {
            while (BaudRate_Mode / 2 > 8)
            {
                BaudRate_Low++;
                BaudRate_Mode = BaudRate_Mode / 2;
            }
            BaudRate_High = --BaudRate_Mode;
            SPI0_BR = BaudRate_High << 4;//数值赋给SPI0_BR的SPPR的D6D5D4位
            SPI0_BR |= BaudRate_Low;//赋值给SPI0_BR的SPR的 D2D1D0位
        }
    }
    else         //初始化SPI1功能
    {
        BSET(SIM_SCGC4_SPI1_SHIFT, SIM_SCGC4);   //打开SPI1模块时钟。
        //引脚复用为SPI1功能
        PORTD_PCR4 = (0 | PORT_PCR_MUX(0x02));     //选择PTD4的SS功能
        PORTD_PCR5 = (0 | PORT_PCR_MUX(0x02));     //选择PTD5的SCK功能
        PORTD_PCR6 = (0 | PORT_PCR_MUX(0x02));     //选择PTD6的MOSI功能
        PORTD_PCR7 = (0 | PORT_PCR_MUX(0x02));     //选择PTD7的MIOS功能

        SPI1_C1 = 0x00;                           //SPI控制寄存器1清零
        BSET(SPI_C1_SPE_SHIFT, SPI1_C1);         //使能SPI模块
        //MSTR=1为主机模式；
        //MSTR=0为从机模式（因MSTR初始值为0，无需更改）
        (MSTR == 1) ? BSET(SPI_C1_MSTR_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_SPIE_SHIFT, SPI1_C1);

        //时钟极性配置，CPOL=0，平时时钟为高电平，反之CPOL=1，平时时钟为低电平
        (0 == CPOL) ? BCLR(SPI_C1_CPOL_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_CPOL_SHIFT, SPI1_C1);

        //CPHA=0时钟信号的第一个边沿出现在8周期数据传输的第一个周期的中央；
        //CPHA=1时钟信号的第一个边沿出现在8周期数据传输的第一个周期的起点。
        (0 == CPHA) ? BCLR(SPI_C1_CPHA_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_CPHA_SHIFT, SPI1_C1);
        //SSOE为1,MODFEN为1，配置本模块为自动SS输出
        BSET(SPI_C1_SSOE_SHIFT, SPI1_C1);
        //对SPI0的C1寄存器配置为主机模式、从机选择自动模式并使能SPI0模块。

        //BSET(SPI_C1_SPIE_SHIFT,SPI1_C1);  //开本模块的SPI中断
        SPI1_C2 = 0x00U;
        if (MSTR == 1)  //主机模式
            BSET(SPI_C2_MODFEN_SHIFT, SPI1_C2);
        SPI1_BR = 0x00U;
        //重新设置波特率
        BaudRate_High = 0;
        BaudRate_Low = 0;
        BaudRate_Mode = 24000 / BaudRate;   //取除数用于寄存器中数据计算
        if (BaudRate_Mode <= 8)
        {
            SPI0_BR = (BaudRate_Mode - 1) << 4;
        }
        else
        {
            while (BaudRate_Mode / 2 > 8)
            {
                BaudRate_Low++;
                BaudRate_Mode = BaudRate_Mode / 2;
            }
            BaudRate_High = --BaudRate_Mode;
            SPI0_BR = BaudRate_High << 4;//数值赋给SPI0_BR的SPPR的D6D5D4位
            SPI0_BR |= BaudRate_Low;   //赋值给SPI0_BR的SPR的 D2D1D0位
        }
    }
}

//=====================================================================
//功能说明：SPI发送一字节数据。
//函数参数：No：模块号，可用参数可参见gec.h文件
//       data：     需要发送的一字节数据。
//函数返回：0：发送失败；1：发送成功。
//=====================================================================
uint8_t spi_send1(uint8_t No, uint8_t data)
{
    // 复位后，发送缓冲区标志位为1; 0表示满，1表示空
    // 当向数据寄存器写数据后，自动将发送缓冲区标志位置1
    SPI_MemMapPtr baseadd = SPI_baseadd(No);

    //等待上一个字符发送完毕，发送缓冲区空闲
    while (!(SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK));

    // 将要发送的数据写入到数据寄存器
    // 向数据寄存器写入数据将会写入到发送数据缓冲器，将数据发送出去
    // 读数据寄存器将会读取到接收数据寄存器的值。
    SPI_DL_REG(baseadd) = data;

    for (uint32_t i = 0;i < 65535;i++) {
        // 在一定时间内如果发送缓冲器为空标志变成 1，说明发送缓冲区为空，发送完毕
        if (SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK) {
            return(1);
        }
    }

    //在一定时间内仍然没有返回1,则认为发送失败，返回0。
    return(0);
}

//=====================================================================
//功能说明：SPI发送数据。
//函数参数：No：模块号，可用参数可参见gec.h文件
//       n:     要发送的字节个数。范围为(1~255)
//       data[]:所发数组的首地址。
//函数返回：无。
//=====================================================================
void spi_sendN(uint8_t No, uint8_t n, uint8_t* buff) {

    // // 第一种实现方式
    // SPI_MemMapPtr baseadd = SPI_baseadd(No);
    // for (uint32_t k = 0;k < n;k++) {
    //     //等待上一个字符发送完毕，发送缓冲区空闲
    //     while (!(SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK));
    //     SPI_DL_REG(baseadd) = data[k];

    //     // 此处感觉不需要清SPTEF位，也就是将SPTEF置1
    //     // 因为向数据寄存器写数据之后，会自动将SPTEF置1
    //     SPI_S_REG(baseadd) |= SPI_S_SPTEF_MASK;    //清除SPTEF位
    // }

    // 第二种实现方式，通过调用spi_send1()也可以实现上述功能
    for (uint32_t i = 0; i < n; i++) {
        spi_send1(No, buff[i]);
    }

    // 发送转义字符0, 告诉接收方一串字符串发送完毕
    spi_send1(No, '\0');
}

//=====================================================================
//功能说明：SPI接收一个字节的数据
//函数参数：No：模块号，可用参数可参见gec.h文件
//函数返回：接收到的数据。
//=====================================================================
uint8_t spi_receive1(uint8_t No)
{
    SPI_MemMapPtr baseadd = SPI_baseadd(No);

    // 接收缓冲寄存器满标志（SPRF）
    // SPRF = 0, 接收缓冲寄存器无可用数据，SPRF = 1, 接收缓冲寄存器有可用数据
    // 读取接收缓冲寄存器之后，该标志会清0
    while (!(SPI_S_REG(baseadd) & SPI_S_SPRF_MASK)); // 当有可用数据时，跳出while循环

    return SPI_DL_REG(baseadd);
}

//=====================================================================
//功能说明：SPI接收数据。当n=1时，就是接受一个字节的数据……
//函数参数：No：模块号，可用参数可参见gec.h文件
//        n:    要发送的字节个数。范围为(1~255),
//       data[]:接收到的数据存放的首地址。
//函数返回：1：接收成功,其他情况：失败。
//=====================================================================

uint32_t spi_receiveN(uint8_t No, uint8_t* buff) {
    // 设置一个静态变量，记录当前接收字符个数
    static uint32_t index = 0;

    buff[index++] = spi_receive1(No);

    // 当接收到转义字符'\0'时， 说明发送方已经发送完毕
    // 此时返回接收的字符串的长度，同时index归0, 方便接收下一个字符串
    if (buff[index - 1] == '\0') {
        index = 0;
        return strlen(buff);
    }

    // 如果不是字符'\0', 说明还有数据，返回 0
    return 0;
}


//=====================================================================
//功能说明：打开SPI接收中断。
//函数参数：No：模块号，可用参数可参见gec.h文件
//函数返回：无。
//=====================================================================
void spi_enable_re_int(uint8_t No)
{
    //enable_irq (No+10);
    NVIC_EnableIRQ(No + 10);
}

//=====================================================================
//功能说明：关闭SPI接收中断。
//函数参数：No：模块号，可用参数可参见gec.h文件
//函数返回：无。
//=====================================================================
void spi_disable_re_int(uint8_t No)
{
    //disable_irq (No+10);
    NVIC_DisableIRQ(No + 10);
}
