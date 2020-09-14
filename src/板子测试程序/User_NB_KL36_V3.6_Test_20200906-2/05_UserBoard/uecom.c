//===================================================================
//文件名称：uecom.c（uecom驱动源程序文件）
//===================================================================
#include "uecom.h"    //包含对应驱动头文件，说明在头文件中

//文件级变量
#define AT_reBuf_length 300
static uint8_t AT_reBuf[300];   //存放UE的AT指令的返回结果
//static uint8_t AT_reFlag = 0;   //AT指令成功返回的标志,成功时该值为接收到的AT指令返回的数据长度
static uint8_t AT_reLength = 0; //AT指令返回的数据长度
static uint8_t AT_haveCommand=0;//标志当前是否有AT指令发送
static uint8_t frameHead[2] = "V!"; //帧头
static uint8_t frameTail[2] = "S$"; //帧尾
//存储初始化后读取到的IMSI号
static uint8_t IMSI[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
//存储初始化后读取到的IMEI号 
static uint8_t IMEI[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  

static uint8_t locationFlag=0;      //接收到一行有效定位信息标志
static uint8_t locationData[100];   //存放接收到的一行有效定位信息


static uint8_t reip[16];    //远程更新IP地址  【2019/04/29】 CC 增
static uint8_t report[6];   //远程更新端口号
static uint8_t reFlag = 0;  //远程更新标识

//                            "1234567890123456789012345";
static uint8_t uecomType[25] = "AHL-NB-IoT-KL36       ";
static uint8_t uecomVersion[5] = "V3.6";



//======================内部函数声明=======================================
uint8_t uecom_sendCmd(uint8_t *cmd,uint16_t maxDelayMs,uint16_t maxTimes);
void uint_to_str(uint32_t ch,char *str);
void frameEncode(uint8_t *imsi,uint8_t *data,uint16_t dlen,
		         uint8_t *frame,uint16_t *framelen);
uint8_t uecom_internal_getIMEI(uint8_t *dest);
uint8_t uecom_internal_getIMSI(uint8_t *dest);
uint8_t uecom_recv(uint8_t ch,uint16_t *dataLen, uint8_t *data);
uint8_t uecom_recvLocation(uint8_t ch, uint8_t *data);
uint8_t uecom_transparentRecv(uint8_t ch,uint16_t *dataLen, uint8_t *data);
void ArrayCopy(uint8_t * dest,uint8_t * source,uint16_t len);
void uecom_Dly_ms(uint16_t ms);
uint16_t ue_crc16(uint8_t *ptr,uint16_t len);

//======================内部函数声明结束===================================

//========================变量声明========================================
//函数中使用到的AT指令，【20180815】

#define AT        "AT\r\n"             //测试与通信模组通讯的指令
#define IPR       "AT+IPR=115200\r\n"  //将模块波特率设置为115200
#define ATE_0     "ATE0\r\n" //关闭回显（发送该命令之后，UE模块不会把原来的命令返回）
#define GSN       "AT+GSN\r\n"          //获取设备的IMEI号指令
#define CIMI      "AT+CIMI\r\n"         //获取设备的IMSI号指令

#define CFUN      "AT+CFUN=1\r\n"       //设置电话功能
#define COPS      "AT+COPS=1,2,\"46011\"\r\n"//设置运营商,46011表示的是电信的
//#define COPS      "AT+COPS=1,2,\"46000\"\r\n"//设置运营商,46000表示的是移动的
#define MSPCHSC   "AT*MSPCHSC=1\r\n"         //设置NB支持的算法，旧算法
#define CPSMS     "AT+CPSMS=0\r\n"           //关闭 PSM 功能
#define EGACT_0   "AT+EGACT=0,1,\"ctnb\"\r\n"//去激活PDP,其中APN是电信的
#define EGACT_1   "AT+EGACT=1,1,\"ctnb\"\r\n"//激活PDP,其中APN是电信的
#define CSQ       "AT+CSQ\r\n"               //获取信号强度

#define ESOC      "AT+ESOC=1,1,1\r\n"        //创建TCP套接字
#define ESOSETRPT "AT+ESOSETRPT=1\r\n"   //设置接收数据的回显形式为“原始数据”
#define ESOCL     "AT+ESOCL=0\r\n"       //关闭套接字
#define ESOCL_1   "AT+ESOCL=1\r\n"       //关闭套接字1
#define MENGINFO  "AT*MENGINFO=0\r\n"    //获取当前小区信息

#define ZGMODE_1 "AT+ZGMODE=1\r\n"    //设置GPS定位模式（AGPS模式）
#define ZGNMEA_2 "AT+ZGNMEA=2\r\n"    //设置GPS数据NMEA上报格式模式（RMC格式）
#define ZGRUN_2 "AT+ZGRUN=2\r\n"      //开启GPS服务（追踪获取数据）
#define METIME  "AT+CCLK?\r\n"       //获取基站时间
//#define ZRST    "AT+ZRST\r\n"        //模块复位

//AT命令是否执行成功的标志
#define OK        "OK"
#define ERROR     "ERROR"
//========================变量声明结束====================================

//=====================================================================
//函数返回：无
//参数说明：state:通信模组电源控制命令。可取值为（使用宏常数）：
//         UECOM_OFF：关闭通信模组电源；
//         UECOM_ON：打开通信模组电源；
//         UECOM_REBOOT：重启通信模组（先关闭，延时，再开启）。
//功能概要：控制通信模块供电状态
//内部调用：gpio_init，delay_ms
//修改日期：【20180718】,LXD
//=====================================================================
void uecom_power(uint8_t state)    //控制通信模块供电状态
{
    switch(state)
    {
    case UECOM_OFF:
        gpio_init(POWER_CONTROL,1,0);break;  //关闭
    case UECOM_ON:
        gpio_init(POWER_CONTROL,1,1);break;  //打开
    case UECOM_REBOOT:
        gpio_init(POWER_CONTROL,1,0);        //关闭
        uecom_Dly_ms(2000);                  //延时2秒
        gpio_init(POWER_CONTROL,1,1);break;  //打开
    }
}


//====================================================================
//函数返回： 0：成功；1：与UE模块串口通讯失败；2：获取SIM卡的IMSI号失败；
//参数说明： 无
//功能概要：uecom模块初始化。（1）建立与通讯模组的串口通讯；
//                          （2）获取SIM卡的IMSI号。
//修改日期：【20180718】,LXD
//====================================================================
uint8_t uecom_init(void)
{
    //（1）变量声明和赋初值
    uint8_t ret_val ;
    //（2）使用串口与通信模组通信，首先初始化串口
    uart_init(UART_UE, 115200);  //初始化UE模块串口，波特率为115200
    uart_enable_re_int(UART_UE); //打开UE模块串口中断
    uecom_Dly_ms(10);
    //（3）测试与通信模块的通讯
    if(uecom_sendCmd((uint8_t *)AT,100,2))
    {
    	uecom_power(UECOM_REBOOT);   //给UE模块重新供电
    	uecom_Dly_ms(9000);
    }
	if(uecom_sendCmd((uint8_t *)AT,100,3))
	    goto uecom_init_err1;     //UE模块通讯失败
    //（4）关闭回显（发送AT命令之后，UE模块不会把原来的命令返回）
    if(uecom_sendCmd((uint8_t *)ATE_0,200,3))
        goto uecom_init_err1;    //关闭回显失败
    //（5）查询模块的IMEI,并将IMEI号放在dest数组的前15个字节
    if(uecom_internal_getIMEI(IMEI))
        goto uecom_init_err1;    //IMEI查询失败
    //（6）查询模块的IMSI,并将IMSI号放在dest数组的15-29下标的空间中
    if(uecom_internal_getIMSI(IMSI))
        goto uecom_init_err2;    //IMSI查询失败
    ret_val = 0;
    goto uecom_init_exit;
     //错误退出
uecom_init_err1:
    ret_val = 1;            //与UE模块串口通讯失败
    goto uecom_init_exit;
uecom_init_err2:
    ret_val = 2;            //获取SIM卡的IMSI号失败
    goto uecom_init_exit;
    //退出处
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//函数返回： 无
//参数说明： 无
//功能概要：断开Socket连接
//修改日期：【20190503】,
//=====================================================================
void uecom_linkOff()
{
    uart_init(UART_UE, 115200);  //初始化UE模块串口，波特率为115200
    uart_enable_re_int(UART_UE); //打开UE模块串口中断
	//先关闭一下套接字0（防止通信模组未断电已经建立套接字）
	uecom_sendCmd((uint8_t *)ESOCL,4000,1);  
	//先关闭一下套接字1（防止通信模组未断电已经建立套接字）
	uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    
}

//=====================================================================
//函数返回： 0：成功建立与铁塔建立连接；1：连接不上铁塔；
//参数说明：无
//功能概要：与网络运营商的基站（铁塔）建立连接
//修改日期：【20180718】,LXD
//=====================================================================
uint8_t uecom_linkBase(void)
{
	//（1）变量声明、赋初值
	uint8_t ret=1;
	//（2）设置电话功能为全功能
	if(uecom_sendCmd((uint8_t *)CFUN,1500,2))
		goto uecom_linkBase_exit;      //设置电话功能为全功能失败
	//（3）设置网络运营商
	uecom_sendCmd((uint8_t *)COPS,1500,3);
//	while(uecom_sendCmd((uint8_t *)COPS,1500,3));
	//（4）设置NB支持的扰码算法
	uecom_sendCmd((uint8_t *)MSPCHSC,500,2);
	//（5）关闭 PSM 功能（因为不涉及功耗测试）
	if(uecom_sendCmd((uint8_t *)CPSMS,200,2))
		goto uecom_linkBase_exit;      //关闭 PSM 功能失败
	//（6）激活PDP
	if(uecom_sendCmd((uint8_t *)EGACT_1,10000,1))
	{
		uecom_sendCmd((uint8_t *)EGACT_0,10000,1);
		uecom_Dly_ms(8000);
		if(uecom_sendCmd((uint8_t *)EGACT_1,10000,2))
			goto uecom_linkBase_exit;  //激活PDP失败
	}
	//至此，没有失败退出，成功！
	ret = 0;
uecom_linkBase_exit:
	    return ret;
}

//=====================================================================
//函数返回：
//参数说明： IP:待连接更新服务器的IP地址； port:待连接更新服务器的端口号
//功能概要：配置更新服务器的IP地址和端口号
//修改日期：【20190426】,CC
//=====================================================================
void uecom_remoteConfig(uint8_t *ip,uint8_t *port)
{
	strcpy((char *)reip,(const char *)ip);
	strcpy((char *)report,(const char *)port);
	reFlag = 1;

}

//=====================================================================
//函数返回： 0：成功建立TCP连接；1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址； port:待连接服务器的端口号
//功能概要：与指定的服务器和端口建立TCP连接。
//修改日期：【20180718】,LXD
//=====================================================================
uint8_t uecom_linkCS(uint8_t *ip,uint8_t* port)
{
    //（1）变量声明和赋初值
    uint8_t i;
//    uint8_t flag;
    uint8_t ret_val=1;
    uint8_t ESOCON[50] = "";    //用存放建立TCP连接的指令
    uint8_t loc;
    //（2）组建建立TCP连接的指令
    strcat((char *)ESOCON,"AT+ESOCON=0,");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<5; i++)//添加端口号（最多5位）
    {
        if(port[i] == 0)
        {
            break;
        }
        ESOCON[loc+i] = port[i];
    }
    strcat((char *)ESOCON,",\"");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<15; i++)//添加端IP（最多15位）
    {
        if(ip[i] == 0)
        {
           break;
        }
        ESOCON[loc+i] = ip[i];
    }
    strcat((char *)ESOCON,"\"\r\n");
    //（3）创建TCP套接字
    uecom_sendCmd((uint8_t *)ESOCL,4000,1);    //先关闭一下套接字（防止通信模组未断电已经建立套接字）
    if(uecom_sendCmd((uint8_t *)ESOC,4000,1))
    {
    	uecom_sendCmd((uint8_t *)ESOCL,4000,1);    //关闭套接字
    	if(uecom_sendCmd((uint8_t *)ESOC,1000,1))
    	{
    		goto uecom_linkCS_exit;
    	}
    }
    //（4）建立TCP连接
    if(uecom_sendCmd(ESOCON,4000,2))
    {
		goto uecom_linkCS_exit;
    }
    //（5）设置接收数据的回显形式为“原始数据”
    if(uecom_sendCmd((uint8_t *)ESOSETRPT,4000,2))
    {
		goto uecom_linkCS_exit;
    }
    ret_val = 0;
    //【2019/04/29】 CC 增
	if(reFlag == 1)
	{
		//连接用户cs成功后连接更新服务器
		uecom_linkRemoteUpdate(reip,report);
	}
    //退出处
uecom_linkCS_exit:
    return ret_val;
}

//=====================================================================
//函数返回： 0：成功建立TCP连接；1：建立TCP连接失败
//参数说明： IP:待连接服务器的IP地址； port:待连接服务器的端口号
//功能概要：与指定的服务器和端口建立TCP连接。（使用套接字1）
//修改日期：【20190504】,CC
//注：uecom_RemoteUpdate要在调用uecom_linkCS之后使用
//=====================================================================
uint8_t uecom_linkRemoteUpdate(uint8_t *ip,uint8_t* port)
{
    //（1）变量声明和赋初值
    uint8_t i;
//    uint8_t flag;
    uint8_t ret_val=1;
    uint8_t ESOCON[50] = "";    //用存放建立TCP连接的指令
    uint8_t loc;
    //（2）组建建立TCP连接的指令
    strcat((char *)ESOCON,"AT+ESOCON=1,");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<5; i++)//添加端口号（最多5位）
    {
        if(port[i] == 0)
        {
            break;
        }
        ESOCON[loc+i] = port[i];
    }
    strcat((char *)ESOCON,",\"");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<15; i++)//添加端IP（最多15位）
    {
        if(ip[i] == 0)
        {
           break;
        }
        ESOCON[loc+i] = ip[i];
    }
    strcat((char *)ESOCON,"\"\r\n");
    //（3）创建TCP套接字
    //先关闭一下套接字0（防止通信模组未断电已经建立套接字）
    uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    
    if(uecom_sendCmd((uint8_t *)ESOC,4000,1))
    {
    	uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    //关闭套接字
    	if(uecom_sendCmd((uint8_t *)ESOC,1000,1))
    	{
    		goto uecom_linkRemoteUpdate_exit;
    	}
    }
    //（4）建立TCP连接
    if(uecom_sendCmd((uint8_t *)ESOCON,4000,2))
    {
		goto uecom_linkRemoteUpdate_exit;
    }
    //（5）设置接收数据的回显形式为“原始数据”
    if(uecom_sendCmd((uint8_t *)ESOSETRPT,4000,2))
    {
		goto uecom_linkRemoteUpdate_exit;
    }
    ret_val = 0;

    //退出处
    uecom_linkRemoteUpdate_exit:
    return ret_val;
}

//=====================================================================
//函数返回：  0：发送成功；1：开启发送模式失败；2：数据发送失败
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP通道发送出去。最多500个字节。
//修改日期：【20180718】,LXD
//=====================================================================
uint8_t uecom_send(uint16_t length, uint8_t *data)
{
    //（1）变量声明，赋初值
    uint8_t ret_val;
    char dataLen[5]="";        //存放帧长(字符串格式)
    char ESOSENDRAW[24]= "";   //存放发开启发送数据的指令
    uint8_t frame[500];         //存放数据帧
    uint16_t frameLen;          //存放帧长
    frameEncode(IMSI,data, length,frame,&frameLen);//组帧
    uint_to_str(frameLen,dataLen);  //将待发送数据的长度转换为字符串格式
    //（2）开启发送模式
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=0,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");
    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //（3）延时等待开启发送模式
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_send_exit;      //开启发送模式失败
	}
    //（4）开始发送数据
    uart_sendN(UART_UE ,frameLen,frame);
    //（5）判断数据是否发送成功
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_send_exit;         //数据发送失败
    }
    //至此数据发送成功
    ret_val = 0;
    //【2019/04/29】 CC 增
	if(reFlag == 1)
	{
		//重新连接用户cs成功后连接更新服务器
		uecom_linkRemoteUpdate(reip,report);
		//向更新程序发送心跳包
		uecom_updateSend(9,(uint8_t *)"heartbeat");
	}
    uecom_send_exit:
    return ret_val;
}

//=====================================================================
//函数返回：  0：发送成功；1：开启发送模式失败；2：数据发送失败
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：将数据通过已经建立的TCP通道发送出去。
//         最多500个字节。（使用套接字0发送）
//修改日期：【20190504】,CC
//=====================================================================
uint8_t uecom_updateSend(uint16_t length, uint8_t *data)
{
    //（1）变量声明，赋初值
    uint8_t ret_val;
    char dataLen[5]="";        //存放帧长(字符串格式)
    char ESOSENDRAW[24]= "";   //存放发开启发送数据的指令
    uint8_t frame[100];         //存放数据帧
    uint16_t frameLen;          //存放帧长
//    uint8_t i,ch,flag,count;
//    uint8_t sendBack[50];
    frameEncode(IMSI,data, length,frame,&frameLen);//组帧
    uint_to_str(frameLen,dataLen);   //将待发送数据的长度转换为字符串格式
    //（2）开启发送模式
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=1,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");
    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //（3）延时等待开启发送模式
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_updateSend_exit;      //开启发送模式失败
	}
    //（4）开始发送数据
    uart_sendN(UART_UE ,frameLen,frame);
    //（5）判断数据是否发送成功
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_updateSend_exit;         //数据发送失败
    }
    //至此数据发送成功
    ret_val = 0;
    uecom_updateSend_exit:
    return ret_val;
}


//=====================================================================
//函数名称：uecom_transparentSend
//函数返回：  0：发送成功；1：开启发送模式失败；2：数据发送失败
//参数说明： data:待发送数据缓存区，传入参数
//		           length:待发送数据的长度
//功能概要：透明发送，将数据通过已经建立的TCP通道发送出去。最多500个字节。
//修改日期：【20180922】,LXD
//=====================================================================
uint8_t uecom_transparentSend(uint16_t length, uint8_t *data)
{
    //（1）变量声明，赋初值
    uint8_t ret_val;
    char dataLen[5]="";        //存放帧长(字符串格式)
    char ESOSENDRAW[24]= "";   //存放发开启发送数据的指令
//    uint8_t frame[500];         //存放数据帧
//    uint16_t frameLen;          //存放帧长
//    frameEncode(IMSI,data, length,frame,&frameLen);//组帧
    uint_to_str(length,dataLen);
    //（2）开启发送模式
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=0,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");

    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //（3）延时等待开启发送模式
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_send_exit;      //开启发送模式失败
	}
    //（4）开始发送数据
    uart_sendN(UART_UE ,length,data);
    //（5）判断数据是否发送成功
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_send_exit;         //数据发送失败
    }
    //至此数据发送成功
    ret_val = 0;
    uecom_send_exit:
    return ret_val;
}


//====================================================================
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：接收到的网络数据长度；
//                   recvData：存储接收到的网络数据
//功能概要：本函数需要放在串口中断中。并需要传入串口中断接收到的数据。
//  本构件的所有功能实现均依赖该api。本api实现的功能：
//  （1）接收网络发送来的数据;（2）构件内部使用本api与模块进行数据交互
//修改日期：【20180718】,LXD
//=====================================================================
void uecom_interrupt(uint8_t ch,uint16_t *length,uint8_t recvData[])
{
	//保存数据到缓冲区中
	if(AT_haveCommand)
	{
		AT_reBuf[AT_reLength] = ch;    //存储接收到的数据到缓冲区AT_reBuf中
		//接收到的数据下标递增
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;
		AT_reBuf[AT_reLength] = 0;
	}
	uecom_recv(ch,length,recvData); //处理来自服务器的数据
	if(locationFlag==0)
	{
		uecom_recvLocation(ch,locationData);//处理定位信息
	}
}


//====================================================================
//函数返回：无
//参数说明：ch：串口中断接收到的数据；length：接收到的网络数据长度；
//                   recvData：存储接收到的网络数据
//功能概要：透传的中断处理函数，本函数需要放在串口中断中。
//         并需要传入串口中断接收到的数据。
//  本构件的所有功能实现均依赖该api。本api实现的功能：
//  （1）接收网络发送来的数据;（2）构件内部使用本api与模块进行数据交互
//修改日期：【20180922】,LXD
//=====================================================================
void uecom_transparentInterrupt(uint8_t ch,uint16_t *length,
		uint8_t recvData[])
{
	//保存数据到缓冲区中
	if(AT_haveCommand)
	{
		AT_reBuf[AT_reLength] = ch;    //存储接收到的数据到缓冲区AT_reBuf中
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;//接收到的数据下标递增
		AT_reBuf[AT_reLength] = 0;
	}
	uecom_transparentRecv(ch,length,recvData); //处理来自服务器的数据
	if(locationFlag==0)
	{
		uecom_recvLocation(ch,locationData);//处理定位信息
	}
}
//=====================================================================
//函数返回：0：操作GNSS成功；1：操作GNSS失败
//参数说明：state：设置GNSS的开关状态。1：热启动；2：温启动；3：冷启动；
//          建议默认使用冷启动。
//功能概要：设置GNSS的状态。开启或关闭GNSS，并设定开启方式。
//修改日期：【20180902】；
//=====================================================================
uint8_t uecom_gnssSwitch (uint8_t state)
{
	//（1）定义使用到的变量
	uint8_t  ret;
	ret = 1;    //默认返回失败
	//（2）根据传入值操作GNSS
	switch(state)
	{
	case 1:
        if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//设置GPS定位模式（AGPS模式）
        	goto uecom_gnssSwitch_exit;
    	if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//设置GPS数据NMEA上报格式模式（RMC格式）
    	{
    		goto uecom_gnssSwitch_exit;
    	}

		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//开启循环获取定位信息
			goto uecom_gnssSwitch_exit;
        break;
	case 2:
		if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//设置GPS定位模式（AGPS模式）
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//设置GPS数据NMEA上报格式模式（RMC格式）
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//开启循环获取定位信息
			goto uecom_gnssSwitch_exit;
		break;
	case 3:
		if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//设置GPS定位模式（AGPS模式）
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//设置GPS数据NMEA上报格式模式（RMC格式）
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//开启循环获取定位信息
			goto uecom_gnssSwitch_exit;
		break;
	default:
		break;
	}
	//至此，本函数运行完成
	ret = 0;
	uecom_gnssSwitch_exit:
	return ret;
}


//====================================================================
//函数返回：0：获取定位信息成功；1：没有获得定位信息
//参数说明：data：存储获得的GNSS相关信息。采用结构体的方式，共包含4个成员：
//       time（15个字节的uint8_t数组）,例如:“20180706155132”表示2018年7月6日15:51:32
//       latitude（double类型），纬度信息；longitude（double），经度信息；
//       speed（double类型），速度，单位为：米每秒。
//       attitude（double类型），海拔高度，单位为：米
//功能概要：获得与GNSS定位相关的信息。
//修改日期：【20180902】
//=====================================================================
uint8_t uecom_gnssGetInfo (UecomGnssInfo *data)   // UecomGnssInfo为存储gnss信息的结构体数据
{
	//（1）定义本函数使用的变量
	uint8_t i,j,ret;
	uint8_t *p;
	double a,b,c,d;
	//（2）判断是否获取到有效的定位数据
	ret = 1;        //设置默认返回1
	if(locationFlag==0)
		goto uecom_gnssGetInfo_exit;
	//（3）从通信模组返回数据中解析出UTC时间
	//（3.1）提取日期
	p =(uint8_t *) strstr((const char *)locationData,(const char *)",");   //获得字符','第一次所在的位置
	j=0;                        //用来标记逗号
	while(j<9)                  //定位到第9个逗号
	{
		if(*p==',')
			j++;
		p++;
	}
	data->time[0]='2';//填充数字
	data->time[1]='0';
	data->time[6]=*p;          //提取日期
	data->time[7]=*(++p);
	data->time[4]=*(++p);
	data->time[5]=*(++p);
	data->time[2]=*(++p);
	data->time[3]=*(++p);
	//（4.2）提取时间
	p =(uint8_t*) strstr((const char *)locationData,(const char *)",");   //获得字符','第一次所在的位置
	i=8;                        //用来标记读到的数据下标
	while(i<14)                 //提取时间
	{
		p++;
		data->time[i]=*p;
		i++;
	}
	data->time[i]=0;                   //添加空操作符组成字符串
	//至此，获取UTC时间成功
	//（4）提取位置信息
	//（4.1）提取出维度信息
	p = (uint8_t *)strstr((const char *)locationData,(const char *)",");   //获得字符','第一次所在的位置
	j=0;                        //用来标记逗号
	while(j<3)                  //定位到第3个逗号
	{
		if(*p==',')
			j++;
		p++;
	}
	a=((*p)-'0')*10.0;
	a=a+((*(++p))-'0');


	b = ((*(++p))-'0')*10.0;
	b =b+ ((*(++p))-'0');

	++p;
	++p; //跳过'.'
	c = 0.1;
	b = b + ((*p) - '0')*c;
	while((*(++p))!=',')
	{
		c *= 0.1;
		b = b + ((*p) - '0')*c;
	}
	++p;    //跳过','
	if((*p) == 'N')
		d = 1;
	else
		d = -1;
	data->latitude = (a + b/60.0)*d;
	//（4.2）提取出经度信息
	++p;
	++p;   //跳过逗号，跳到经度信息位置

	a=((*p)-'0')*100.0;
	a=a+((*(++p))-'0')*10.0;
	a=a+((*(++p))-'0');

	b = ((*(++p))-'0')*10.0 ;
	b=+ ((*(++p))-'0');


	++p;
	++p;   //跳过'.'
	c = 0.1;
	b = b + ((*p) - '0')*c;
	while((*(++p))!=',')
	{
		c *= 0.1;
		b = b + ((*p) - '0')*c;
	}
	++p;    //跳过','
	if((*p) == 'E')
		d = 1;
	else
		d = -1;
	data->longitude = (a + b/60.0)*d;
	//（5）从通信模组返回数据中解析出速度信息，并存入传入的结构体
	if(strstr((const char *)locationData,(const char *)"E,") != NULL)
		 p =(uint8_t *) strstr((const char *)locationData,(const char *)"E,");
	else
		 p =(uint8_t *) strstr((const char *)locationData,(const char *)"W,");
	p++;
	p++;//跳到对地速度处
	a = 0;
	while((*p)!='.')
	{
		a = a*10 + (*p - '0');
		p++;
	}
	p++;//跳过点
	b = 0.1;
	a = a + (*p - '0')*b;
	p++;
	while((*p)!=',')
	{
		b *= 0.1;
		a = a + (*p - '0')*b;
		p++;
	}
	data->speed = a*0.5144444;
	//（6）从通信模组返回数据中解析出海拔信息，并存入传入的结构体
	data->attitude = 0;//无海拔信息
	//至此，数据解析完成
	locationFlag=0;    //清空接收到一行有效定位信息标志
	ret = 0;
	uecom_gnssGetInfo_exit:
	return ret;
}

//====================================================================
//函数返回：0：获取基站信息成功；1：获取信号强度失败；
//参数说明：retData：存储返回的信息,最少分配20个字节。
//          信息组成：信号强度(1个字节)+基站号（19个字节）
//功能概要：获取与基站相关的信息：信号强度和基站号
//修改日期：【20180815】,WB
//=====================================================================
uint8_t uecom_baseInfo (uint8_t retData [20])
{
	//（1）变量声明和赋初值
	uint8_t i,j,*p,ret;
	//（2）获取信号强度，最多获取三次，间隔1秒
	ret = 1;           //设置错误返回为1
	for(j=0;j<3;j++)
	{
		//（2.1）向模组发送获取信号强度的指令
		if( uecom_sendCmd((uint8_t *)CSQ,500,3))
			goto uecom_baseInfo_exit;
		//（2.2）从接收到的数据中解析出信号强度，转为uint8_t格式，并赋给*signalPower
		i = strstr((const char *)AT_reBuf,(const char *)"+CSQ: ")-(char*)AT_reBuf;
		i+=5;
		retData[0]= 0;
		while(AT_reBuf[i]!=','&&i<AT_reLength)
		{
			if(AT_reBuf[i]>='0' && AT_reBuf[i]<='9')
				retData[0] = retData[0]*10 + (AT_reBuf[i]-'0');
			i++;
		}
		if(retData[0]!=0)
	    {
			retData[0] = (uint16_t)100*(retData[0])/31;
			break;
		}
		uecom_Dly_ms(1000);
	}
	if(j==3)goto uecom_baseInfo_exit;  //若3次均失败，则认为获取信号强度失败
    //（3）获得基站号
	ret = 2;           //设置错误返回为2
	//（3.1）向模组发送获取基站号的指令
	if(uecom_sendCmd((uint8_t *)MENGINFO,500,3))
		goto uecom_baseInfo_exit;
	//（3.2）从接收到的数据中解析出基站号，并存入retData数组中
	p =(uint8_t *) strstr((const char *)AT_reBuf,(const char *)"*MENGINFOSC:");   //获得字符':'所在的位置，其后为所需的数据。
	j = 1;
	i = 0;
	strncpy((char *)retData+1,(const char *)"460,11,",7);
	j+=7;
	while(i<3)//跳过到第三个逗号
	{
		if(*p == ',')
			i++;
		p++;
	}
	++p;//跳到cell ID处
	while((*p)!='\"')//提取cell ID
	{
		retData[j++] = *p;
		p++;
	}
	retData[j++] =',';
	while(i<9)//跳过到第九个逗号
	{
		if(*p == ',')
			i++;
		p++;
	}
	++p;//跳到TAC处
	while((*p)!='\"')//提取TAC
	{
		retData[j++] = *p;
		p++;
	}
	if(j<=0)  //未能成功解析到数据
		goto uecom_baseInfo_exit;   //退出函数
	//最后一个逗号删除，并设置为字符串结束符
	retData[j]=0;
    //（4）运行至此，说明执行成功
    ret = 0;          //返回值设为0
    uecom_baseInfo_exit:
    return ret;
}

//====================================================================
//函数返回：0：获取模组信息成功；1：获取模组信息失败
//参数说明：retData：存储返回的信息,最少分配40个字节。
//          信息组成：IMEI (20个字节)+IMSI（20个字节）
//功能概要：获得需要与模块相关的信息，包括：IMEI号，IMSI号
//修改日期：【20180718】,LXD
//=====================================================================
uint8_t uecom_modelInfo (uint8_t retData[40])
{
	int i;
	//（1）从本构件的全局变量中取出IMEI并赋值
	for(i=0;i<15;i++)
		retData[i]=IMEI[i];
	retData[i] = 0;    //结束符
	//（2）从本构件的全局变量中取出IMSI并赋值
	for(i=20;i<35;i++)
		retData[i]=IMSI[i-20];
	retData[i] = 0;   //结束符
	return 0;
}

//======================================================================
//函数返回：无
//参数说明：type:金葫芦型号
//功能概要：获取金葫芦型号
//======================================================================
uint8_t uecom_typeGet(uint8_t *type)
{
	memcpy(type,uecomType,sizeof(uecomType));
	return 0;
}

//======================================================================
//函数返回：无
//参数说明：biosVer:金葫芦版本（通信模组型号）
//功能概要：获取金葫芦版本
//======================================================================
void uecom_version(uint8_t *version)
{
	memcpy(version,uecomVersion,sizeof(uecomVersion));
}

//====================================================================
//函数返回：0：获得get请求成功；1：初始化http失败；2：传递url参数失败；
//			3：设置网络失败；4：开启网络失败；5：建立连接失败；
//          6：发送请求失败；7：获得返回失败；
//参数说明：ip:目标服务器地址；port :目标地址；
//		 	url:get请求的内容。result:get请求返回的结果，
//          数组长度由预计返回的长度（用户应已知返回内容）*1.5来决定。
//功能概要：发起http的get请求，并将返回结果存储在result中
//=====================================================================
uint8_t uecom_httpGet (uint8_t ip[],uint8_t port[],uint8_t url[],
		uint8_t result[])
{
	return 1;
}

//------以下为本构件调用的内部函数--------------------------------------------
//====================================================================
//函数返回：0：指令发送成功;1：指令发送失败
//参数说明：cmd：需要发送的AT指令的首地址或者"wait"或者"SEND";
//        个字节;maxDelayMs:每次命令最多等待时间;maxTimes:最多发送命令次数。
//注："wait"判断是否可以发送数据（即出现‘CONNECT’）；
//    "SEND"用于判断发送数据是否成功；
//功能概要：发送AT指令并获取返回信息。
//修改日期：【2018-06-30】,LXD
//====================================================================
uint8_t uecom_sendCmd(uint8_t *cmd,uint16_t maxDelayMs,uint16_t maxTimes)
{
	//中断方式实现
    //（1）变量声明和赋初值
	uint8_t ret;
	uint16_t i,j,count;
	//存放发送AT指令后所含有的字符串，默认为"OK"
	uint8_t waitStr[30]="OK";      
	AT_reLength = 0;
	//清空字符串（当在接收中断里再接收一个字节时，在其后的位置赋值0）
	AT_reBuf[0]=0;
	//用于判断发送完开启发送数据指令“AT+ESOSENDRAW”后，
	if(strcmp(( const char *)cmd,( const char *)"wait")==0)
		//是否收到CONNECT，从而去发送数据
		strcpy((char *)waitStr,( const char *)"CONNECT"); 
		
		//cmd="wait";waitstr="CONNECT"
		//cmd;waitstr="OK"
		
	AT_haveCommand=1;   //置位AT命令标志
	uecom_Dly_ms(10);  //防止串口AT命令发送过于频繁
	j = maxDelayMs/100;
	uart_enable_re_int(UART_UE);//开启中断
	for(count=0;count<maxTimes;count++)
	{
		//如果是AT指令
		if(strcmp(( const char *)cmd,( const char *)"SEND") 
			&& strcmp(( const char *)cmd,( const char *)"wait"))
		{
			AT_reLength = 0;
			//清空字符串（当在接收中断里再接收一个字节时，在其后的位置赋值0）
			AT_reBuf[0]=0;
			uart_send_string(UART_UE,cmd); //通过串口发送AT指令
		}
		//等待maxDelayMs毫秒
		for(i=0; i<j;i++)
		{
	        uecom_Dly_ms(100);
	        if(strstr(( const char *)AT_reBuf,( const char *)waitStr))
	        {
	        	ret = 0;
	        	goto uecom_sendCmd_exit;
	        }
	        if(strstr(( const char *)AT_reBuf,( const char *)ERROR))
	        {
	        	uecom_Dly_ms(200);//缓冲100毫秒
	        	break;
	        }
		}
		//至此，说明未接收到正确返回。
    	j=j*2; //若失败，下一次发送指令时，等待时间加倍
    	if (j>(maxDelayMs/100)*3) j=(maxDelayMs/100)*3;
    	
	}
	ret = 1;
	uecom_sendCmd_exit:
	AT_haveCommand=0;
	return ret;
}

//==================================================================
//函数返回： 0：获取IMEI成功；1：获取IMEI号失败；2：解析IMEI号失败
//参数说明：dest：存放返回的IMEI号，15位
//功能概要：获取设备IMEI号
//内部调用：uecom_sendCmd
//修改日期：【2018-06-30】,LXD
//==================================================================
uint8_t uecom_internal_getIMEI(uint8_t *dest)
{
    //（1）变量声明和赋初值
    uint8_t ret_val;
    uint8_t *p,*start,*end;
    //（2）获取IMEI号
    dest[15] = 0;        //字符串的结尾符
    if(uecom_sendCmd((uint8_t *)GSN,200,4))
    {
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)GSN,200,5))
		{
			ret_val = 1;
			goto uecom_internal_getIMEI_exit;      //获取IMEI号失败
		}
    }

    //strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。
    //如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。

    //（3）将IMEI号存储在dest数组中
    p =(uint8_t *) strstr((const char *)AT_reBuf,(const char *)"OK");
    while(p>=AT_reBuf&&(*p<'0'||*p>'9'))p--;    //查找到最后一个数字所在的位置
    end=p;
    while(p>=AT_reBuf && *p>='0' && *p<='9')p--;//查找到第一个数字所在的位置
    start=p+1;
    if(start>end)
    {
        ret_val = 2;
        goto uecom_internal_getIMEI_exit;      //返回的数据中不含IMEI号
    }

    //void *memcpy(void *dest, const void *src, size_t n);
    //从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中

    memcpy(dest,start,end-start+1);//拷贝读取到的数据
    dest[end-start+1]=0;//结束符
    ret_val = 0;
    //退出处
uecom_internal_getIMEI_exit:
    return ret_val;
}

//==================================================================
//函数返回：0：获取IMSI成功；1：获取IMSI失败
//参数说明：dest：存放返回的IMSI号，15位
//功能概要：获取设备IMSI号
//内部调用：uecom_sendCmd
//修改日期：【2018-06-30】,LXD
//==================================================================
uint8_t uecom_internal_getIMSI(uint8_t *dest)
{
    //（1）变量声明和赋初值
    uint8_t ret_val;
//    uint8_t i,k;
    uint8_t *p,*start,*end;
    dest[14] = 0;
    //（2）获取IMSI号
	if(uecom_sendCmd((uint8_t *)CIMI,6000,2))
	{
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)CIMI,6000,1))
		{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;     //获取IMSI号失败
		}
	}

	//strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。
	//如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。

    //（3）将IMSI号存储在dest数组中
    p = (uint8_t *)strstr((const char *)AT_reBuf,(const char *)"OK");
    while(p>=AT_reBuf&&(*p<'0'||*p>'9'))p--;    //查找到最后一个数字所在的位置
    end=p;
    while(p>=AT_reBuf && *p>='0' && *p<='9')p--;//查找到第一个数字所在的位置
    start=p+1;
    if(start>end)
    {
        ret_val = 2;
        goto uecom_internal_getIMSI_exit;       //返回的数据中不含IMSI号
    }

    //void *memcpy(void *dest, const void *src, size_t n);
    //从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中

    memcpy(dest,start,end-start+1);             //拷贝读取到的数据
    dest[end-start+1]=0;                        //结束符
    ret_val = 0;
    //退出处
uecom_internal_getIMSI_exit:
    return ret_val;
}

//====================================================================
//函数返回：无
//参数说明：ch:待转换的正整数        *str：存放转换后的字符串
//功能概要：将无符号整数转换为字符串
//====================================================================
void uint_to_str(uint32_t ch,char *str)
{
    int i,j,sign;
    char s[10];
    if((sign=ch)<0)//记录符号
        ch=-ch;//使n成为正数
    i=0;
    do{
           s[i++]=ch%10+'0';//取下一个数字
    }
    while ((ch/=10)>0);//删除该数字
    if(sign<0)
      s[i++]='-';
    for(j=i-1;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
           str[i-1-j]=s[j];
    str[i]=0;
}

//====================================================================
//功能概要：实现待发送数据的组帧,将待发送数据加上帧头、帧长、帧尾以及校验信息
//       frame=帧头+IMSI+data长度+data+CRC校验码+帧尾
//参数说明：imsi：发送数据的设备的IMSI号
//       data:待组帧的数据头指针
//       dlen:待组帧的数据长度
//       frame:组帧之后的待发送数据帧
//       framelen:组帧后的数据长度
//函数返回：无
//====================================================================
void frameEncode(uint8_t *imsi,uint8_t *data,uint16_t dlen,
                 uint8_t *frame,uint16_t *framelen)
{
    uint16_t crc,len;

    //组帧操作
    //帧头
    frame[0] = frameHead[0];
    frame[1] = frameHead[1];
    len = 2;
    //IMSI号
    strncpy((char *)frame+2,(const char *)imsi,15);
    len += 15;
    //data长度
    frame[len++] = dlen>>8;
    frame[len++] = dlen;
    //data
    memcpy(frame+19,data,dlen);
    len += dlen;
    //CRC校验码
    
    //对IMSI号、data长度和data进行CRC校验
    crc = ue_crc16((uint8_t *)(frame+2),dlen+17);
    frame[len++] = crc>>8;
    frame[len++] = crc;
    //帧尾
    frame[len++] = frameTail[0];
    frame[len++] = frameTail[1];
    frame[len] = 0;
    *framelen = len;
}

//====================================================================
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：正在解帧；2接收数据错误
//参数说明：ch：串口接收到的数据(1字节)，*dataLen:存储接收到的数据长度，
//      *data:将接收到的数据存放到该数组中;
//功能概要：对接收到来自服务器的数据进行解帧，并将有效数据存入data数组之中，
//          由串口的uart中断调用。
//修改日期：【2018-06-30】,LXD
//====================================================================
uint8_t uecom_recv(uint8_t ch,uint16_t *dataLen, uint8_t *data)
{
    static uint16_t index1 = 0;   //帧索引
    static uint16_t length = 0;  //数据长度
    uint8_t ret_val;
    uint16_t i;
    uint8_t MCRC[2];
    uint16_t mcrc;
    //如果未遇到帧头或者未收到帧头后面的数据(即不是数据帧)
    if((index1 == 0 && ch != frameHead[0]) ||
      (index1 == 1 && ch != frameHead[1]))
    {
    	index1 = 0;
        length = 0;
        ret_val = 2;     //接收数据错误
        goto uecom_recv_exit;
    }
    //至此说明是来自服务器的数据
    data[index1++] = ch;  //存储帧数据
    if(index1 == 19)      //读取data长度
    {
        length = ((uint16_t)data[17]<<8) + data[18];
    }
    //接收到的数据达到一帧长度。length+23为整个帧长度
    if(length != 0 && index1 >= length+23)
    {
        //CRC校验
        mcrc = ue_crc16((uint8_t *)data+2,length+17);
        MCRC[0] = (mcrc>>8)&0xff;
        MCRC[1] = mcrc&0xff;
        if(data[index1-2]!=frameTail[0] ||
        		data[index1-1]!= frameTail[1] //未遇到帧尾
            || MCRC[0] != data[length+19] ||
			MCRC[1] != data[length+20])       //CRC检验错误
        {
        	index1 = 0;
            length = 0;
            ret_val = 2;         //接收数据错误
            goto uecom_recv_exit;
        }
        for(i=0;i<length;i++)
        {
            data[i] = data[i+19];//19为有效字节前的数据长度
        }
        *dataLen = length;

        index1 = 0;
        length = 0;
        ret_val = 0;             //接收到了通过TCP/UDP发来的数据
        goto uecom_recv_exit;
    }
    *dataLen = 0;
    ret_val = 1;//正在解帧
uecom_recv_exit:
    return ret_val;
}

//=====================================================================
//函数返回：0：接收到了通过TCP/UDP发来的数据；1：未接收到数据
//参数说明：ch:串口接收到的数据
//                   *dataLen:存储接收到的数据长度
//                   *data:将接收到的数据存放到该数组串中;
//功能概要：透传接收函数，将接收到的数据存入data数组之中，由串口的uart中断调用。
//修改日期：【20180922】,LXD
//=====================================================================
uint8_t uecom_transparentRecv(uint8_t ch,uint16_t *dataLen, uint8_t *data)
{
	static uint8_t flag = 0;
	static uint16_t recvLength = 0;
	static uint16_t index = 0;
	uint8_t ret_val = 1;
	switch(flag)
	{
	case 0:    if(ch=='+')flag=1; else flag = 0;break;
	case 1:    if(ch=='E')flag=2; else flag = 0;break;
	case 2:    if(ch=='S')flag=3; else flag = 0;break;
	case 3:    if(ch=='O')flag=4; else flag = 0;break;
	case 4:    if(ch=='N')flag=5; else flag = 0;break;
	case 5:    if(ch=='M')flag=6; else flag = 0;break;
	case 6:    if(ch=='I')flag=7; else flag = 0;break;
	case 7:    if(ch=='=')flag=8; else flag = 0;break;
	case 8:    if(ch=='0')flag=9; else flag = 0;break;
	case 9:    if(ch==',')flag=10; else flag = 0;recvLength = 0;break;
	case 10:   if(ch==','){flag=11;index=0;}else recvLength=recvLength*10+ch-'0';break;
	case 11:
	//无帧格式
		if(index<recvLength)
		{
			data[index++]=ch;
		}
		else
		{
			ret_val = 0;
			*dataLen = recvLength;
			flag = 0;
		}
		break;
	}
    return ret_val;
}

//====================================================================
//函数名称：uecom_recvLocation
//函数返回：0：接收到了定位信息；1：接收数据错误
//参数说明：ch：串口接收到的数据(1字节); *data:将接收到的数据存放到该数组中;
//功能概要：将接收到的一行定位信息存入data数组之中，由串口的uart中断调用。
//修改日期：【2018-07-17】,LXD
//====================================================================
uint8_t uecom_recvLocation(uint8_t ch, uint8_t *data)
{
	static uint16_t index2 = 0;   //数据索引
    uint8_t ret_val,*p,j;
    ret_val=0;
    //如果接收的前三个字节不是$GN,则认为不是定位信息
    if((index2 == 0 && ch != '$')
    	||(index2 == 1 && ch != 'G')
		||(index2 == 2 && ch != 'N')
		||(index2 == 3 && ch != 'R')
		||(index2 == 4 && ch != 'M')
		||(index2 == 5 && ch != 'C')
		)
    {
    	index2 = 0;
        ret_val = 1;     //接收数据错误
        goto uecom_recv_exit;
    }
    //至此说明是定位信息
    data[index2++] = ch;      //存储接收到定位信息
    if(ch=='\n') //如果接收到一行有效定位信息
    {
    	if(data[17]=='A')
    	{
    		p =(uint8_t *) strstr((const char *)data,(const char *)",");   //获得字符','第一次所在的位置
    		j=0;                    //用来标记逗号
    		while(j<9)              //定位到第9个逗号
    		{
    			if(*p==',')
    				j++;
    			p++;
    		}
    		if((*p)!=',')        //如果接收到日期（第九个逗号后是日期）
    		{
    			locationFlag=1;//置位接收到一行有效定位信息标志
    			index2 = 0;
    			ret_val = 0;     //接收到一行定位信息
    			goto uecom_recv_exit;
    		}
    	}
		index2 = 0;
		ret_val = 1;
    }
uecom_recv_exit:
        return ret_val;
}

//======================================================================
//函数返回：成功：返回时间戳（1970.1.1至此所经历的秒数）   失败：返回0
//参数说明：无
//功能概要：获取附近基站的时间，以时间戳的格式返回，要求在通信模组完成
//          基站初始化后才能使用
//======================================================================
void uecom_getTime(uint8_t dateform[20])
{
	//（1）变量声明和赋初值
	uint8_t m;
	uint8_t statu;
	uint16_t i,j;
	//年月日时分秒 十进制
	int tyear=0;
	uint16_t tmonth;
	uint16_t tday;
	uint16_t thour;
	uint16_t tmin;
	uint16_t tsec;
	uint8_t datetime[20];
	//（2）发送获取时间指令
	for(m=0;m<50;m++) AT_reBuf[m]=0;
	if(uecom_sendCmd((uint8_t *)METIME,200,1))
	{
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)METIME,200,1))
		{
			//ret_val = 0;
			goto uecom_getTime_exit;     //获取IMSI号失败
		}
	}
	uecom_Dly_ms(50);
	i=0;
	statu=0;
	//过滤多余字符
	while(AT_reBuf[i]!=0 && i<50)
	{
		if(statu==0)
		{
			if(AT_reBuf[i]=='L') statu=1;
		}
		else if(statu==1)
		{
			if(AT_reBuf[i]=='K') statu=2;
			else statu=0;
		}
		else if(statu==2)
		{
			if(AT_reBuf[i]==':') statu=3;
			else statu=0;
		}
		else if(statu==3)	break;
		i++;
	}
	if(statu!=3) return;  //字符串错误，返回初始时间
	ArrayCopy(AT_reBuf,AT_reBuf+i,50-i);     //字符串前移
	tyear=0;
	tmonth=0;
	tday=0;
	thour=0;
	tmin=0;
	tsec=0;
	j=0;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!='/'&&j<50)
	{
		tyear=tyear*10+AT_reBuf[j]-'0';
		j++;
	}
	datetime[0]=tyear/1000;
	datetime[1]=(tyear/100)%10;
	datetime[2]=(tyear/10)%100;
	datetime[3]=tyear%10;
    datetime[4]=0;
	j++;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!='/')
	{
		tmonth=tmonth*10+AT_reBuf[j]-'0';
		j++;
	}
	datetime[5]=tmonth/10;
	datetime[6]=tmonth%10;
    datetime[7]=0;
	if(AT_reBuf[j]==0) return;
	j++;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!=',')
	{
		tday=tday*10+AT_reBuf[j]-'0';
		j++;
	}
	datetime[8]=(tday/10);
	datetime[9]=(tday%10);
    datetime[10]=0;
	if(AT_reBuf[j]==0) return;
	j++;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!=':')
	{
		thour=thour*10+AT_reBuf[j]-'0';
		j++;
	}
	//加上东八区的时间
	thour=thour+8;
	if(thour>24)
	{
		thour=thour-24;
		tday=tday+1;
		if(tday>28 && tmonth==2)
		{
			if(tyear%4==0)
				tday=tday+1;
			else
			{
				tday=1;
				tmonth=tmonth+1;
			}
		}
		if(tday>30)
		{
			switch(tmonth)
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
						tday=tday+1;break;
				case 4:
				case 6:
				case 9:
				case 11:
				{
					tday=1;
					tmonth=tmonth+1;
					if(tmonth>12)
					{
						tmonth=1;
						tyear=tyear+1;
					}
					break;
				}
			}
		}
	}
	datetime[0]=tyear/1000;
	datetime[1]=(tyear/100)%10;
	datetime[2]=(tyear/10)%100;
    datetime[3]=tyear%10;
	datetime[4]=0;
	datetime[8]=(tday/10);
	datetime[9]=(tday%10);
    datetime[10]=0;
	datetime[11]=(thour/10);
	datetime[12]=(thour%10);
    datetime[13]=0;
	if(AT_reBuf[j]==0) return;
	j++;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!=':')
	{
		tmin=tmin*10+AT_reBuf[j]-'0';
		j++;
	}
	datetime[14]=(tmin/10);
	datetime[15]=(tmin%10);
    datetime[16]=0;
	if(AT_reBuf[j]==0) return;
	j++;
	while(AT_reBuf[j]!=0&&AT_reBuf[j]!='G')
	{
		tsec=tsec*10+AT_reBuf[j]-'0';
		j++;
	}
	datetime[17]=(tsec/10);
	datetime[18]=(tsec%10);
	datetime[19]=0;
    for(int t=0;t<20;t++)
    {
        if ((t!=4) && (t!=7) && (t!=10) && (t!=13)&&(t!=16)&&(t!=19))
        {
        	dateform[t]=datetime[t]+0x30;  //转为ASCII字符
        }
    }
	AT_reLength = 0;
	AT_reBuf[0]=0;//清空字符串（当在接收中断里再接收一个字节时，在其后的位置赋值0）
	uecom_Dly_ms(100);
	//退出处
   uecom_getTime_exit:
   return;
}

//=====================================================================
//功能概要：将数据进行16位的CRC校验，返回校验后的结果值
//参数说明：ptr:需要校验的数据缓存区
//      len:需要检验的数据长度
//函数返回：计算得到的校验值
//=====================================================================
uint16_t ue_crc16(uint8_t *ptr,uint16_t len)
{
	uint16_t i,j,tmp,crc16;

	crc16 = 0xffff;
	for(i = 0;i < len;i++)
	{
		crc16 = ptr[i]^crc16;
		for(j = 0;j< 8;j++)
		{
			tmp = crc16 & 0x0001;
			crc16 = crc16>>1;
			if(tmp)
				crc16 = crc16^0xa001;
		}
	}
	return crc16;
}


//=====================================================================
//函数返回：无
//参数说明：dest：复制后存放的数组；source：被复制的数组；len:复制的长度
//功能概要：从源数组复制指定长度的内容到目标数组
//=====================================================================
void ArrayCopy(uint8_t * dest,uint8_t * source,uint16_t len)
{
	uint16_t i = 0;
	for(i=0;i<len;i++)
		dest[i]=source[i];
}

//======================================================================
//函数返回：无
//参数说明：无
//功能概要：延时 - 毫秒级
//======================================================================
void uecom_Dly_ms(uint16_t ms)
{
	for (uint32_t ys = 0; ys < (7000*ms); ys++)  __asm ("nop");
}