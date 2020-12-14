//=====================================================================
//文件名称：i2c.c
//功能概要：i2c底层驱动构件源文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：  2019-05-09  V2.0
//适用芯片：KL25、KL26、KL36
//=====================================================================
#include  "i2c.h"

static const I2C_MemMapPtr I2C_BASE_TABLE[2] = I2C_BASE_PTRS;
static const IRQn_Type I2C_IRQ_TABLE[2] = { I2C0_IRQn,I2C1_IRQn };
void Delay_ms(uint16_t u16ms);
//**********************对外接口函数************************************
//=====================================================================
//函数名称：i2c_init
//功能概要：初始化IICX模块。默认为100K.
//参数说明：No:模块号,可用参数可参见gec.h文件
//         Mode：模式  1：主机     0：从机
//         address：本模块初始化地址    范围(1~255)
//         BaudRate：为波特率，其单位为Kb/s，其取值为25,50,75,100K
//函数返回：无
//=====================================================================
void i2c_init(uint8_t No, uint8_t Mode, uint8_t address, uint8_t BaudRate)
{
	ModeSelect = Mode;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	if (No == 0)
	{
		BSET(SIM_SCGC4_I2C0_SHIFT, SIM_SCGC4); //开时钟门
		PORTB_PCR2 = PORT_PCR_MUX(0x2);      //使能IIC0_SCL
		PORTB_PCR3 = PORT_PCR_MUX(0x2);      //使能IIC0_SDA
	}
	else
	{
		BSET(SIM_SCGC4_I2C1_SHIFT, SIM_SCGC4); //开时钟门
		PORTC_PCR1 = PORT_PCR_MUX(0x2);      //使能IIC1_SCL
		PORTC_PCR2 = PORT_PCR_MUX(0x2);      //使能IIC1_SDA
	}
	i2c_ptr->C1 = 0X00;   //初始化C1
	i2c_ptr->FLT |= I2C_FLT_STOPF_MASK;//清停止标志位
	i2c_ptr->S |= I2C_S_ARBL_MASK | I2C_S_IICIF_MASK; //初始化S
	if (1 == ModeSelect)   //主机模式
	{
		switch (BaudRate)
		{
		case 25:
			i2c_ptr->F = 0x9f;
			break;
		case 50:
			i2c_ptr->F = 0x5f;
			break;
		case 75:
			i2c_ptr->F = 0x25;
			break;
		case 100:
			i2c_ptr->F = 0x1f;
			break;
		default:
			i2c_ptr->F = 0x1f;
			break;
		}
		BSET(I2C_C1_IICEN_SHIFT, i2c_ptr->C1);//开i2c0模块使能
	}
	else            //从机模式
	{
		i2c_ptr->A1 = address << 1;
		BSET(I2C_C1_IICEN_SHIFT, i2c_ptr->C1);//使能I2C模块
		BCLR(I2C_C1_TX_SHIFT, i2c_ptr->C1);//TX = 0,MCU设置为接收模式
		BCLR(I2C_C1_MST_SHIFT, i2c_ptr->C1);//MST位由1变成0，设置为从机模式
	}
}

//=====================================================================
//函数名称：  i2c_read1
//功能概要：  读取1个字节数据
//参数说明：  No:模块号，可用参数可参见gec.h文件
//          slaveAddress:读取的目标地址
//          command： 读取命令，取值范围为0~255。
//                   若无读取命令则该参数传0xFFFF
//          Data:带回收到的一个字节数据
//函数返回：  1：表示读取失败；0：读取成功
//函数说明: 该函数为对外函数，可以根据命令读取到相应的1个字节，若想读取2个
//         或以上字节数数据，请使用I2C_readN函数，不建议使用循环调用此函数
//=====================================================================
uint8_t i2c_read1(uint8_t No, uint8_t slaveAddress, uint16_t command, uint8_t* Data)
{
	uint8_t temp;
	i2c_start(No);               //发送开始信号
	if (command != 0xFFFF)
	{
		temp = (slaveAddress << 1) & 0xfe;	// 最低位为0，数据方向是写
		i2c_write(No, temp);          //将地址和数据方向位发送给从机

		if (i2c_wait(No) == 1)        //等待完成
			return 1;                    //失败
		if (i2c_get_ack(No) == 1)     //判断是否是ACK
			return 1;                    //若是非应答信号则返回失败

		// command = (uint8_t)command;		// 数据存放位置
		i2c_write(No, (uint8_t)command);       //将命令发送给从机
		if (i2c_wait(No) == 1)        //等待完成
			return 1;                    //失败
		i2c_restart(No);             //重新开始
	}

	temp = (slaveAddress << 1) | 0x01;		// 最低位为1，数据方向是读
	i2c_write(No, temp);  //将地址和数据方向位发送给从机

	if (i2c_wait(No) == 1)        //等待完成
		return 1;                    //失败
	if (i2c_get_ack(No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败

	i2c_changeDir(No, 0);         //改变数据传送方向，主机由发送变成接收
	I2C_M0NACK(No);            //发送NACK
	i2c_read(No);                //空读，触发下个数据传递
	if (i2c_wait(No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(No);                //发送停止信号
	*Data = i2c_read(No);  //读取数据，操作完成，可以在后面加少量延时
	return 0;
}

//=====================================================================
//函数名称：i2c_readN.
//功能说明： 读取N个字节，在这里N最小为2。
//函数参数： i2cNO:i2c模块号 ，可用参数可参见gec.h文件
//          slaveAddress:读取的目标地址
//          command： 读取命令，取值范围为0~255。
//                   若无读取命令则该参数传0xFFFF
//          Data：存放的数据首地址
//          DataNum：要读取的字节数（大于等于2）
//函数返回： 1：表示读取失败；0：读取成功
//=====================================================================
uint8_t i2c_readN(uint8_t No, uint8_t slaveAddress, uint16_t command, uint8_t* Data, uint8_t DataNum)
{
	uint8_t i, temp;
	i2c_start(No);               //发送开始信号
	if (command != 0xFFFF)
	{
		temp = (slaveAddress << 1) & 0xfe;
		i2c_write(No, temp);          //将地址和方向位发送给从机
		if (i2c_wait(No) == 1)        //等待完成
		{
			return 1;
		}
		if (i2c_get_ack(No) == 1)     //判断是否是ACK
		{
			return 1;
		}                                //若是非应答信号则返回失败

		// command = (uint8_t)command;
		i2c_write(No, (uint8_t)command);       //将命令发送给从机
		if (i2c_wait(No) == 1)        //等待完成
			return 1;                //失败
								   //失败
		Delay_ms(100);
		i2c_restart(No);             //在restart之前可以增加少许延时
	}
	temp = (slaveAddress << 1) | 0x01;
	i2c_write(No, temp);          //将地址和方向位发送给从机
	if (i2c_wait(No) == 1)        //等待完成
	{
		return 1; //失败
	}
	if (i2c_get_ack(No) == 1)     //判断是否是ACK
	{
		return 1; //失败
	}
	i2c_changeDir(No, 0);         //改变方向
	I2C_M0ACK(No);             //发送应答位ACK
	i2c_read(No);                //空读，触发下个数据传递
	if (i2c_wait(No) == 1)        //等待完成
	{
		return 1; //失败
	}
	for (i = 0; i < DataNum - 2; i++)
	{
		Data[i] = i2c_read(No);
		if (i2c_wait(No) == 1)    //等待完成
		{
			return 1;//失败
		}
	}
	I2C_M0NACK(No);            //发送非应答位ACK
	Data[i++] = i2c_read(No);
	if (i2c_wait(No) == 1)        //等待完成
	{
		return 1;
	}
	i2c_stop(No);                //发送停止信号
	Data[i] = i2c_read(No);      //读取数据，操作完成，可以在后面加少量延时
	return 0;
}

//=====================================================================
//函数名称： i2c_write1
//功能概要： 发送1个字节数据
//参数说明： No:模块号，可用参数可参见gec.h文件
//          slaveAddress：发送目标的地址
//          Data:待发送的的1个字节数据
//          command:地址（命令）
//函数返回：  0，成功发送一个字节；1，发送一个字节失败
//函数说明:  1：表示发送失败；0：发送成功
//=====================================================================
uint8_t i2c_write1(uint8_t No, uint8_t slaveAddress, uint8_t Data, uint16_t command)
{
	i2c_start(No);               //发送开始信号
	slaveAddress = (slaveAddress << 1) & 0xfe;
	i2c_write(No, slaveAddress);  //将地址和方向位发送给从机
	/*
		主机及发送给从机地址和数据方向位之后，要等待从机的应答信号
		如果有应答信号，则会有中断挂起，i2c_wait()检测到有中断挂起会返回0表示应答成功
		如果一段时间之后还没有应答信号，i2c_wait()会返回1表示失败
	*/
	if (i2c_wait(No) == 1)        //应答信号，为低电平说明成功
		return 1;                    //失败

	/*
		应答信号分两种，一种是正常的应答信号，表示从机希望主机继续发送接下来的数据
		一种是非应答信号（有的书为了避免歧义，称为应答非信号），表示不希望继续传送数据
	*/
	if (i2c_get_ack(No) == 1)     //判断是否是ACK
		return 1;                    //若是非应答信号则返回失败
	if (0xFFFF != command)
	{
		i2c_write(No, command);  // 发送数据存放位置
		if (i2c_wait(No) == 1)        //等待完成
			return 1;                    //失败
		if (i2c_get_ack(No) == 1)     //判断是否是ACK
			return 1;
	}
	i2c_write(No, Data);          // 发送真正的数据
	if (i2c_wait(No) == 1)        //等待完成
		return 1;                    //失败
	i2c_stop(No);                //发送停止信号
}

//=====================================================================
//函数名称： I2C_writeN
//功能概要： 发送N个字节数据
//参数说明： No:模块号，可用参数可参见gec.h文件
//          slaveAddress：发送目标的地址
//          *Data:待发送N字节数据首地址
//          DataNum：发送的字节数N
//函数返回：  0，成功发送一个字节；1，发送一个字节失败
//函数说明: 1：表示发送失败；0：发送成功
//=====================================================================
uint8_t i2c_writeN(uint8_t No, uint8_t slaveAddress, uint8_t* Data, uint8_t DataNum) {
	uint8_t i;
	i2c_start(No);               //发送开始信号

	slaveAddress = (slaveAddress << 1) & 0xfe;

	i2c_write(No, slaveAddress);  //将地址和方向位发送给从机

	if (i2c_wait(No) == 1) return 1;    //等待完成, 如果失败就返回1
	if (i2c_get_ack(No) == 1)	return 1;      //判断是否是ACK，若是非应答信号则返回失败

	for (i = 0; i < DataNum; i++) {
		i2c_write(No, Data[i]);   //发送真正的数据给从机
		if (i2c_wait(No) == 1) return 1;    //等待完成, 如果失败就返回1
	}

	// 发送字符串结束标志
	i2c_write(No, '\0');
	if (i2c_wait(No) == 1) return 1;    //等待完成, 如果失败就返回1

	i2c_stop(No);                //发送停止信号
	return 0;                        //成功
}

//==============================================================================
//函数名称：i2c_enable_re_int.
//功能说明：打开i2c的IRQ中断。
//函数参数：No:i2c模块号，可用参数可参见gec.h文件
//函数返回：无。
//==============================================================================
void i2c_enable_re_int(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	i2c_ptr->S |= I2C_S_IICIF_MASK;
	i2c_ptr->C1 |= I2C_C1_IICIE_MASK;
	NVIC_EnableIRQ(I2C_IRQ_TABLE[No]);
}

//==============================================================================
//函数名称：i2c_disable_re_int
//功能说明：关闭i2c的IRQ中断。
//函数参数：No:i2c模块号，可用参数可参见gec.h文件
//函数返回：无。
//==============================================================================
void i2c_disable_re_int(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	i2c_ptr->C1 &= ~I2C_C1_IICIE_MASK;
	NVIC_DisableIRQ(I2C_IRQ_TABLE[No]);
}

//=====================================================================
//函数名称：i2c_clear_re_int
//功能说明：清除i2c的接收中断标志。
//函数参数：No:i2c模块号 ，可用参数可参见gec.h文件
//函数返回：无。
//=====================================================================
void i2c_clear_re_int(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	i2c_ptr->S |= 0x02;
}

//=====================================================================
//函数名称：wait
//功能概要：等待完成
//参数说明：No:模块号,可用参数可参见gec.h文件
//函数返回：1：表示超时，0：表示正常
//=====================================================================
uint8_t i2c_wait(uint8_t No) {
	uint16_t time = 0;
	uint16_t time_out = 0xfffe;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	// 当IICIF为0时没有中断挂起，当IICIF为1时有中断挂起
	while (BGET(I2C_S_IICIF_SHIFT, i2c_ptr->S) == 0) {
		time++;
		if (time > time_out)
			return 1;
	}

	// 通过写1来进行清零
	BSET(I2C_S_IICIF_SHIFT, i2c_ptr->S);
	return 0;
}

//=====================================================================
//函数名称：i2c_read
//功能概要：接收数据
//参数说明：No:模块号,可用参数可参见gec.h文件
//函数返回：寄存器中的数据
//=====================================================================
uint8_t i2c_read(uint8_t No) {
	uint8_t result;
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	//	for(int i=0;i<500000;i++);
	result = i2c_ptr->D;
	return result;
}

//=====================================================================
//函数名称： i2c_write
//功能概要： 发送数据
//参数说明： No:模块号,可用参数可参见gec.h文件
//          data：要写入的数据
//函数返回： 无
//=====================================================================
void i2c_write(uint8_t No, uint8_t data)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	i2c_ptr->D = data;
}

//=====================================================================
//函数名称： i2c_stop
//功能概要： 停止信号
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回： 无
//=====================================================================
void i2c_stop(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BCLR(I2C_C1_MST_SHIFT, i2c_ptr->C1); //将主机改成从机发送停止信号
	BCLR(I2C_C1_TX_SHIFT, i2c_ptr->C1);  //该位接收模式
}

//=====================================================================
//函数名称： i2c_start
//功能概要： 开始信号
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回： 无
//=====================================================================
void i2c_start(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BSET(I2C_C1_TX_SHIFT, i2c_ptr->C1);  //该位接收模式
	BSET(I2C_C1_MST_SHIFT, i2c_ptr->C1); //将主机改成从机发送停止信号
}

//=====================================================================
//函数名称： i2c_restart
//功能概要： 重新开始信号
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回： 无
//=====================================================================
void i2c_restart(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BSET(I2C_C1_TX_SHIFT, i2c_ptr->C1);  //该位接收模式
	BSET(I2C_C1_RSTA_SHIFT, i2c_ptr->C1); //将主机改成从机发送停止信号
}

//=====================================================================
//函数名称I2C_M0ndACK
//功能概要： 发送ACK
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回：无
//=====================================================================
void I2C_M0ACK(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BCLR(I2C_C1_TXAK_SHIFT, i2c_ptr->C1);
}

//=====================================================================
//函数名称I2C_M0ndNACK
//功能概要： 发送NACK
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回：无
//=====================================================================
void I2C_M0NACK(uint8_t No) {
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BSET(I2C_C1_TXAK_SHIFT, i2c_ptr->C1);
}

//=====================================================================
//函数名称： i2c_changeDir
//功能概要： 改变方向。
//参数说明： No:模块号,可用参数可参见gec.h文件
//         direction: 方向 0是接收，1是发送
//函数返回： 无
//=====================================================================
void i2c_changeDir(uint8_t No, uint8_t direction)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	i2c_ptr->C1 &= ~I2C_C1_TX_MASK;
	i2c_ptr->C1 |= I2C_C1_TX(direction);
}

//=====================================================================
//函数名称： i2c_get_ack
//功能概要： 获取应答或者非应答信号
//参数说明： No:模块号,可用参数可参见gec.h文件
//函数返回：0：应答信号；1：非应答信号
//=====================================================================
uint8_t i2c_get_ack(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	uint8_t ack;
	ack = BGET(I2C_S_RXAK_SHIFT, i2c_ptr->S);
	return ack;
}

uint32_t i2c_recv(uint8_t No, uint8_t* buff) {
	static uint32_t index = 0;
	buff[index++] = i2c_read(No);

	// 当接收到字符'\0'时，说明字符串接收成功
	if (buff[index - 1] == '\0') {
		index = 0;
		return strlen(buff);
	}

	return 0;
}

uint8_t i2c_send(uint8_t No, uint8_t slaveAddress, uint8_t* buff, uint8_t len) {

	/*
		第一种实现方方式，调用i2c_init()初始化函数
	*/
	// // 将No模块设置为主机
	// i2c_init(No, 1, masterAddress, baudRate);
	// // 禁止No模块的接收中断，接下来将要发送数据
	// i2c_disable_re_int(No);
	// // No模块向从机发送len个字节数据
	// i2c_writeN(No, slaveAddress, buff, len);
	// // 发送数据完毕后将No设置为从机
	// i2c_init(No, 0, masterAddress, baudRate);
	// // 开启No模块的接收中断，监听从其他主机发送的数据。在接收中断函数中判断是否和自己通信
	// i2c_enable_re_int(No);

	/*
		第二种实现方式，直接修改相关配置寄存器
	*/
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];		// 获取该模块相关配置寄存器：地址，分频，控制，状态，数据等寄存器
	BSET(I2C_C1_TX_SHIFT, i2c_ptr->C1);//TX = 1,MCU设置为发送模式
	BSET(I2C_C1_MST_SHIFT, i2c_ptr->C1);//MST位由0变成1，设置为主机模式
	i2c_disable_re_int(No);		// 设置成主机之后要禁用接收中断

	i2c_writeN(No, slaveAddress, buff, len);	// 发送数据

	BCLR(I2C_C1_TX_SHIFT, i2c_ptr->C1);//TX = 0,MCU设置为接收模式
	BCLR(I2C_C1_MST_SHIFT, i2c_ptr->C1);//MST位由1变成0，设置为从机模式
	i2c_enable_re_int(No);	// 设置成从机之后开启接收中断
}



//======================================================================
//函数名称：Delay_ms
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void Delay_ms(uint16_t u16ms)
{
	uint32_t u32ctr;
	for (u32ctr = 0; u32ctr < (48000 * u16ms); u32ctr++)
	{
		__asm("NOP");
	}
}
