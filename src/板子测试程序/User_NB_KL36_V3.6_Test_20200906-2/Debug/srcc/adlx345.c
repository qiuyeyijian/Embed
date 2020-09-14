//======================================================================
//文件名称：adlx345.c
//功能概要：小灯构件头文件
//制作单位：苏大arm技术中心(sumcu.suda.edu.cn)
//更新记录：20190309 ZYL
//======================================================================

#include "adlx345.h"    //包含adlx345头文件

uint8_t I2CModule;
static const I2C_MemMapPtr I2C_BASE_TABLE[2] = I2C_BASE_PTRS;
static const IRQn_Type I2C_IRQ_TABLE[2] = {I2C0_IRQn,I2C1_IRQn};

uint8_t adlx345_write1(uint8_t No, uint8_t slaveAddress, uint8_t Data,uint16_t command);
void adxl345_start(uint8_t No);
void adxl345_write(uint8_t No,uint8_t data);
uint8_t adxl345_wait(uint8_t No);
uint8_t adxl345_get_ack(uint8_t No);
void adxl345_stop(uint8_t No);
//=======================接口函数=======================================
//======================================================================
//函数名称：adlx345_init
//函数参数：range:测量范围；rate:测量速率；power:电源模式；dataready:
//         DATA_READY中断配置；xOffset:X轴偏移量；yOffset:Y轴偏移量；
//         zOffset:Z轴偏移量；
//函数返回：无
//功能概要：ADLX345加速度传感器初始化。
//======================================================================
void adlx345_init(uint8_t module,uint8_t range,uint8_t rate,uint8_t power,uint8_t dataready,
		uint8_t xOffset,uint8_t yOffset,uint8_t zOffset)
{
	I2CModule=module;
	i2c_init(I2CModule,1,0X9F,100);    //I2C模块初始化
	adlx345_Dly_ms(1);
	adlx345_write1(I2CModule,slaveaddress,range,0X31);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,rate,0x2C);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,power,0X2D);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,dataready,0X2E);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,xOffset,0X1E);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,yOffset,0X1F);
    adlx345_Dly_ms(1);
    adlx345_write1(I2CModule,slaveaddress,zOffset,0X20);
    adlx345_Dly_ms(1);
}


//======================================================================
//函数名称：adlx345_read1
//函数参数：command:读取地址（命令）；data:存放读取的数据
//函数返回：无
//功能概要：ADLX345加速度传感器读取1字节。
//======================================================================
void adlx345_read1(uint8_t command,uint8_t *data)
{
	i2c_read1(I2CModule,slaveaddress,command,data);
}

//======================================================================
//函数名称：adlx345_readN
//函数参数：command:读取地址（命令）；data:存放读取的数据;num:字节数
//函数返回：无
//功能概要：ADLX345加速度传感器读取N字节。。
//======================================================================
void adlx345_readN(uint8_t command,uint8_t *data,uint8_t num)
{
	i2c_readN(I2CModule,slaveaddress,command,data,num);
}
//================================内部函数===============================
//=====================================================================
//函数名称： i2c_write1
//功能概要： 发送1个字节数据
//参数说明： No:模块号，其取值为0，1
//          slaveAddress：发送目标的地址
//          Data:待发送的的1个字节数据
//          command:地址（命令）
//函数返回：  0，成功发送一个字节；1，发送一个字节失败
//函数说明:  1：表示发送失败；0：发送成功
//=====================================================================
uint8_t adlx345_write1(uint8_t No, uint8_t slaveAddress, uint8_t Data,uint16_t command)
{
	adxl345_start(No);               //发送开始信号
	slaveAddress = (slaveAddress << 1) & 0xfe;
	adxl345_write(No,slaveAddress);  //将地址和方向位发送给从机
	if(adxl345_wait(No) == 1)        //等待完成
		return 1;                    //失败
	if(adxl345_get_ack(No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	if(0xFFFF != command)
	{
		adxl345_write(No,command);  //将地址和方向位发送给从机
		if(adxl345_wait(No) == 1)        //等待完成
			return 1;                    //失败
		if(adxl345_get_ack(No) == 1)     //判断是否是ACK
			return 1;
	}
	adxl345_write(No,Data);          //将地址和方向位发送给从机
	if(adxl345_wait(No) == 1)        //等待完成
		return 1;                    //失败
	adxl345_stop(No);                //发送停止信号
}

//=====================================================================
//函数名称： adxl345_start
//功能概要： 开始信号
//参数说明： No:模块号,其取值为0,1
//函数返回： 无
//=====================================================================
 void adxl345_start(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //该位接收模式
	BSET(I2C_C1_MST_SHIFT,i2c_ptr->C1); //将主机改成从机发送停止信号
}

 //=====================================================================
 //函数名称： adxl345_write
 //功能概要： 发送数据
 //参数说明： No:模块号,其取值为0,1
 //        data：要写入的数据
 //函数返回： 无
 //=====================================================================
 void adxl345_write(uint8_t No,uint8_t data)
 {
 	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
 	i2c_ptr->D = data;
 }

 //=====================================================================
 //函数名称：wait
 //功能概要：等待完成
 //参数说明：No:模块号,其取值为0,1
 //函数返回：1：表示超时，0：表示正常
 //=====================================================================
 uint8_t adxl345_wait(uint8_t No)
 {
 	uint16_t time=0;
 	uint16_t time_out = 0xfffe;
 	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
 	while(BGET(I2C_S_IICIF_SHIFT,i2c_ptr->S) == 0)
 	{
 		time++;
 		if(time > time_out)
 			return 1;
 	}
 	BSET(I2C_S_IICIF_SHIFT,i2c_ptr->S);
 	return 0;
 }

//=====================================================================
//函数名称： adxl_get_ack
//功能概要： 获取应答或者非应答信号
//参数说明： No:模块号,其取值为0,1
//函数返回：0：应答信号；1：非应答信号
//=====================================================================
uint8_t adxl345_get_ack(uint8_t No)
{
    I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
    uint8_t ack;
    ack = BGET(I2C_S_RXAK_SHIFT,i2c_ptr->S);
    return ack;
}

//=====================================================================
//函数名称： adxl345_stop
//功能概要： 停止信号
//参数说明： No:模块号,其取值为0,1
//函数返回： 无
//=====================================================================
void adxl345_stop(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1); //将主机改成从机发送停止信号
	BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //该位接收模式
}
void adlx345_Dly_ms(uint32_t ms)
{
    for(uint32_t i= 0; i < (6000*ms); i++) __asm("nop");
}

