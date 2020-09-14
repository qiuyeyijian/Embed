//======================================================================
//文件名称：main.c（应用工程主函数）
//框架提供：苏大arm技术中心（sumcu.suda.edu.cn）
//版本更新：2017.08:1.0, 2019.1:A.10
//功能描述：见本工程的<01_Doc>文件夹下Readme.txt文件
//======================================================================
#define GLOBLE_VAR
#include "includes.h"      //包含总头文件

//【根据实际需要增删】初始化程序后，重新烧录126扇区的值


//----------------------------------------------------------------------

//----------------------------------------------------------------------
//【根据实际需要增删】声明使用到的内部函数
//main.c使用的内部函数声明处
void userData_init(UserData *data);   //初始化用户帧结构体data
void userData_get(UserData *data);    //给用户帧结构体data赋值
void LCD_Showfirst();                 //初始化LCD的显示，主要是标签
void ArrayCpy(uint8_t * dest,uint8_t*source,uint16_t len);
void main_Dly_ms(uint32_t ms);
void main_TimeStamp();
void Test_9_PeripheralUnit();
//----------------------------------------------------------------------
//主函数，一般情况下可以认为程序从此开始运行（实际上有启动过程见书稿）
int main(void)
{
    //（1）启动部分（开头）======
    
    //（1.1）声明main函数使用的局部变量
    uint16_t mi,mj;                  //主程序使用到的16位循环变量
    int mTsi;                       //记录当前触摸按键次数
    uint8_t  mSendFlag;              //发送数据标志位;1:发送;0:不发送
    uint8_t  mRetdata[100];          //存放uecom初始化返回的结果
    uint16_t mUserLen;               //需要发送的字节长度
    uint16_t mFlashLen;              //需要存储flash的字节长度
    uint64_t mRecvCount;             //收到的次数
    uint8_t mString[30];             //数字转文本使用的临时数组
    uint8_t mCmd[2];                 //存储命令
    uint8_t mflag;                   //主循环使用的标志判断变量
    uint8_t mWriteFlashFlag;         //表明是否需要将数据写入flash
    uint8_t mLinkCount;              //表明基站连接次数
    int mLCD_RefreshFlag;            //LCD显示刷新标志
    uint8_t  mSendData[1000];        //待发送数据缓冲区
    uint16_t mSendLen;               //待发送数据的长度
    uint8_t  mLBS[30];               //存储LBS定位信息
    uint8_t  mSec;                   //存放运行时长，单位：秒
    uint8_t  mSecOld;                //存放运行时长，单位：秒
    uint16_t  mSendFreq;  
    uint16_t  mSendFreqOld;  
    uint8_t  mUE_infor[40];           //存储UE信息
    uint8_t mUpdateUE_flag;           //标志位，需要更新模组信息
    
    //（1.2）【不变】关总中断
    DISABLE_INTERRUPTS;
    
    //（1.3）给主程序的临时变量赋初值     
    mTsi=0;                            //清空触摸按键次数
    mSendFlag = 1;                     //默认终端发送数据
    mWriteFlashFlag = 0;               //默认不写入flash
    mLinkCount=0;                      //基站连接次数=0
    mUserLen = sizeof(UserData);      //获得需要发送的字节长度
    mFlashLen = sizeof(FlashData);    //获得存入flash的字节长度
    mRecvCount = 0;                    //清空接收次数
    mLCD_RefreshFlag=0;                //LCD显示刷新标志
    mSendLen=0;
    mSec=0;
    mSecOld=0;
    mSendFreq=0;  
    mSendFreqOld=0; 
    mUpdateUE_flag=1;                  //标志位，需要更新模组信息
    
    //（1.4）给全局变量赋初值
    ArrayCpy(gTimeString,(uint8_t *)"0000-00-00 00:00:00\0",20);
    gCount=0;
    gUserData.touchNum=0;
    
    //（1.5）用户外设模块初始化
    uecom_power(UECOM_OFF);
    gpio_init(LIGHT_RED,GPIO_OUTPUT,LIGHT_ON);    //初始化红灯
    gpio_init(GPIO_TSI,GPIO_INPUT,1);             //GPIO模拟触摸
    uart_init(UART_UE,115200);                    //用户串口初始化
    timer_init(TIMER_USER,20);                    //用户计时器初始化
    flash_init();                                 //初始化flash
    LCD_Init();                                   //初始化LCD   
    adc_init(AD_LIGHT,0);                 		   //初始化AD光照模块
    adc_init(AD_MCU_TEMP,0);					   //初始化AD芯片温度模块
    
    //【画瓢处1】-初始化
    
    //（1.6）使能模块中断
    timer_enable_int(TIMER_USER);                //使能LPTMR中断
    
    //（1.7）【不变】开总中断
    ENABLE_INTERRUPTS;

    
    //（1.6）使能模块中断
    timer_enable_int(TIMER_USER);    //开启定时器中断
    
    //（1.8）给通信模组供电
    uecom_power(UECOM_ON);            
    printf("AHL-IoT-GEC start...  \r\n");
    
    //（1.9）【根据实际需要增删】 主循环前的初始化操作
    //向Flash最后一个扇区写用户参数   
    printf("flash_erase and write...        \r\n");
    flash_erase(MCU_SECTOR_NUM-1);    //擦除最后一个扇区
    //向最后一个扇区写数据
    flash_write((MCU_SECTOR_NUM-1),28,sizeof(FlashData),
    (uint8_t *)flashInit);
    printf("flash_erase and write...OK     \r\n");
    //（1.9.1）读取flash中的配置信息至gFlashData；初始化用户帧数据gUserData
    //读取Flash中126扇区的参数信息到gFlashData中   
    flash_read_logic((uint8_t*)(&gFlashData),
    (MCU_SECTOR_NUM-1),28,sizeof(FlashData));
    userData_init(&gUserData);         //初始化用户帧结构体gUserData
    LCD_Showfirst();                   //LCD显示初始内容
    //（1.9.2）判断复位状态，并将复位状态数据存储到flash中（注意不影响原有数据）
    if (IS_POWERON_RESET)               //冷复位，置零
    gFlashData.resetCount = 0;
    else                                //热复位，则加1 
    {
        gFlashData.resetCount++;
        flash_read_logic((uint8_t*)gcRecvBuf,(MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE);
        flash_erase(MCU_SECTOR_NUM-1);
        ArrayCpy(((uint8_t*)gcRecvBuf+166),(uint8_t*)(gFlashData.resetCount),4);
        flash_write((MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE,(uint8_t*)gcRecvBuf);
    }
    
    //（1.9.3）初始化通信模组，并在LCD上显示初始化过程
    //LCD上一行最多显示28个字节
    printf("金葫芦提示：进行通信模组初始化... \r\n");
    for(;;)                           //初始化通信模组循环
    {
        //通信模组上电并等待延时等待约12秒
        uecom_power(UECOM_ON);        //给通信模组供电
        main_Dly_ms(3000);            //延时6s
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init . uecom_power      ");
        printf("金葫芦提示：AHL Init . 给通信模组供电 \r\n");
        main_Dly_ms(3000);            //延时6s
        //通信模组初始化，包括联网和建立网络连接过程
        //初始化通信模组
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ..  uecom_init     ");
        printf("金葫芦提示：AHL Init .. 初始化通信模组 \r\n");
        mflag =uecom_init();
        if(mflag)
        {
            if (mflag==1)
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init .. AT Error        ");
            printf("金葫芦提示：AHL Init .. AT指令错误！\r\n");
            if (mflag==2)
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init .. SIM Error       ");
            printf("金葫芦提示：AHL Init .. 读SIM卡错误！\r\n");
            uecom_power(UECOM_OFF);  //通信模组重启
            continue;
        }
        //显示设备的IMSI号
        uecom_modelInfo(mRetdata);    //获取通信模组信息
        LCD_ShowString(60,85,BLUE,GRAY,(char *)mRetdata+20);
        //与基站建立连接
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ... uecom_linkBase ");
        printf("AHL Init ...  与基站建立连接   \r\n");
        mflag =uecom_linkBase();
        while(mflag)
        {
            mflag =uecom_linkBase();
            mLinkCount++; //连接次数+1
            LCD_ShowString(6,300,BLUE,GRAY,(char *)IntConvertToStr(mLinkCount,mString));
            printf("与基站建立连接次数：%d\r\n",mLinkCount);
            if(mLinkCount>10)                //与基站连接次数 
            {
                mLinkCount=0;
                uecom_power(UECOM_OFF);    //通信模组重启
                main_Dly_ms(2000);      //延时
                uecom_power(UECOM_REBOOT); //通信模组重启
                main_Dly_ms(2000);      //延时
                mflag =uecom_init();
                break;
            }
        }
        
        if(mflag)
        {
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ...link base Error ");
            uecom_power(UECOM_OFF);    //通信模组重启
            main_Dly_ms(1000);      //延时
            uecom_power(UECOM_REBOOT); //通信模组重启
            main_Dly_ms(1000);      //延时
            continue;
            
        }
        //显示信号强度和小区号（基站号）位置信息 LBS
        for(mi=0;mi<5;mi++)
        {
            mflag = uecom_baseInfo(mRetdata);
            if(mflag)
            {
                main_Dly_ms(1000);
                continue;
            }
            ArrayCpy(mLBS,mRetdata+1,19);  //位置信息保存在mLBS中备用
            LCD_ShowString(60,131,BLUE,GRAY,(char *)mRetdata+1);  //基站位置
            
            LCD_ShowString(170,150,BLUE,GRAY,
            (char *)IntConvertToStr(mRetdata[0],(uint8_t*)mString));
        }
        //与服务器建立连接
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ....uecom_linkCS   ");
        printf("AHL Init .... 与云平台建立连接   \r\n");
        mflag =uecom_linkCS((uint8_t*)gFlashData.serverIP,(uint8_t*)gFlashData.serverPort);
        if(mflag)
        {
            LCD_ShowString(6,300,BLUE,GRAY,"AHL....Link CS-Monitor Error");
            uecom_power(UECOM_REBOOT);  //通信模组重启
            continue;
        }
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ..... Successfully ");
        printf("AHL Init ..... Successfully \r\n");
        break;
    }
    uecom_getTime(gTimeString);      //获取基站时间
    
    LCD_ShowString(49,209,BLUE,GRAY,(char *)gTimeString);     	//在LCD上显示当前时间
    printf("NB-IoT communication OK!  \n");
    //（1）启动部分（结尾）======
    printf("Go to Main Loop\r\n");
    //（2）主循环部分（开头）======
    
    for(;;)
    {
        main_loop: 
        //取秒个位的ASCII码，进行判断
        
        
        
        mSec=gTimeString[18]; 
        if (mSec==mSecOld) continue;   //一秒未到，回到循环开始处，继续循环
        Test_9_PeripheralUnit();
         
        //（2.1）一秒到达之后进行的操作
        mSecOld=mSec;                  //mSecOld变量更新
        //在LCD上显示当前时间、控制红灯闪烁、向PC机输出时间
        LCD_ShowString(49,209,BLUE,GRAY,(char *)gTimeString);    //在LCD上显示当前时间
        gpio_reverse(LIGHT_RED);    //红灯每秒闪烁一次
        printf("当前时间为：%s\r\n",gTimeString);
        
        //（2.1.3）判断是否到达发送数据的时间
        mSendFreq++;
        if (mSendFreq-mSendFreqOld>=gFlashData.sendFrequencySec) 
        {
            mSendFreqOld=mSendFreq; 
            mSendFlag = 1;                         //发送标志置1；
            printf("%d 秒时间到，发送一帧数据！\r\n",(int)gFlashData.sendFrequencySec);
            printf("\r\n");
        }
        
        mj=0;
        for (mi=0;mi<3000;mi++) 
        mj=mj+gpio_get(GPIO_TSI);
        if (mj<2000) 
        {
            mTsi++;
            printf("触摸次数 = %d\r\n", mTsi);
            LCD_ShowString(40,189,BLUE,GRAY,(char *)IntConvertToStr(mTsi,mString));    //LCD上显示TSI触摸次数
            
            printf("\r\n");
            if ((mTsi-gUserData.touchNum)>=3)
            {
                mSendFlag = 1;
                printf("触摸三次，发送一帧数据！\r\n");
                printf("\r\n");
                gUserData.touchNum=mTsi;
                printf("触摸次数 ============= %d\n", mTsi);
                printf("\r\n");
            } 
        }    
        
        //（2.3）若需要执行发送数据操作，则进行下列操作
        if(mSendFlag == 1)
        {	   
            //（2.3.1）更新用户数据为最新数据
            userData_get(&gUserData);      //给用户帧结构体gUserData赋值
            if(mUpdateUE_flag==1)
            {
                mUpdateUE_flag=0;
                uecom_modelInfo(mUE_infor);   //传回UE信息
                ArrayCpy(gUserData.IMEI,mUE_infor,15);
                ArrayCpy(gUserData.IMSI,mUE_infor+20,15);
                uecom_baseInfo(mUE_infor);
                gUserData.signalPower = mUE_infor[0];
                ArrayCpy(gUserData.lbs_location,mUE_infor+1,19);
            }
            
            //（2.3.2）根据命令，获得需要发送的数据
            if(gFlashData.frameCmd[0]=='U'&&gFlashData.frameCmd[1]=='0')
            { 
                mSendLen = mUserLen;
                ArrayCpy(gUserData.cmd,gFlashData.frameCmd,2);
                ArrayCpy(mSendData,(uint8_t *)&gUserData,mSendLen);
            }
            else if(gFlashData.frameCmd[0]=='U'&&gFlashData.frameCmd[1]=='1')
            {
                ArrayCpy(mSendData,gFlashData.frameCmd,2);
                ArrayCpy(mSendData+2,gUserData.IMSI,15);
                //【20200816】
                main_TimeStamp();   //给gUserData.currentTime赋值
                
                ArrayCpy(mSendData+17,(uint8_t *)&gUserData.currentTime,8);
                
                ArrayCpy(mSendData+25,(uint8_t *)&gUserData.mcuTemp,4);
                ArrayCpy(mSendData+29,(uint8_t *)&gUserData.signalPower,1);
                ArrayCpy(mSendData+30,(uint8_t *)&gUserData.bright,2);
                ArrayCpy(mSendData+32,(uint8_t *)&gUserData.touchNum,2);
                ArrayCpy(mSendData+34,gUserData.lbs_location,25);
                //【画瓢处3】更改“U1”命令发送的数据
            }
            //（2.3.3）显示信号强度
            LCD_ShowString(170,150,BLUE,GRAY,"     ");
            LCD_ShowString(170,150,BLUE,GRAY,
            (char *)IntConvertToStr(gUserData.signalPower,(uint8_t *)mString));
            
            mLBS[20]='\0';
            gUserData.serverIP[13]='\0';
            gUserData.IMSI[15]='\0';
            printf("信号强度:%d\n\n",gUserData.signalPower);	
            printf("LBS号:%s\r\n\n",mLBS);
            printf("接收次数:%d\n\n",mRecvCount);
            printf("IP:PT  %s : %s\n\n",gFlashData.serverIP,gFlashData.serverPort);
            printf("Frep(s):%d\n\n",gFlashData.sendFrequencySec);
            printf("IMSI:%s\n\n",gUserData.IMSI);
            
            
            
            //（2.3.4）UE模块发送数据
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Send .                  ");
            
            mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            //【20200818】  发送前确定连接云服务器是否成功，以避免假发送
            uint8_t temp=0 ;                 //连接云服务器次数
            if(temp<3&&(mflag!=0))
            {
                mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
                LCD_ShowString(6,300,BLUE,GRAY,"AHL....Link CS-Monitor Error");
                uecom_power(UECOM_REBOOT);  //通信模组重启
                temp++;
            }
            //【20200818】  三次连接失败，重新初始化
            if(mflag!=0)
            {
                printf("多次连接CS_monitor失败，重新初始化\r\n");
                
            }
            
            
            //结构体的地址可直接强制转为数组的地址
            mflag = uecom_send(mSendLen,mSendData);
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Send ..                 ");
            //if(mflag)  goto main_loop_1;    //数据发送失败，LCD显示提示
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Send Successfully       ");
            goto main_loop_2;
            main_loop_1:
            //数据发送失败提示
            switch(mflag)
            {
                case 1:
                LCD_ShowString(6,300,BLUE,GRAY,(char *)"Send Error:Send Not Start   ");
                break;
                case 2:
                LCD_ShowString(6,300,BLUE,GRAY,(char *)"Send Error:Send Data Not OK ");
                break;
            }
            //重新初始化
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Reinit .                ");
            uecom_power(UECOM_OFF);    //通信模组重启
            main_Dly_ms(3000);         //延时
            uecom_power(UECOM_REBOOT); //通信模组重启
            main_Dly_ms(3000);         //延时
            uecom_init();
            main_Dly_ms(100);
            //进行两次初始化（防错）
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Reinit ..                ");
            if(uecom_init())
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"uecom init success ");
            else
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"uecom init fail ");
            
            if(uecom_linkBase())
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"link base success ");
            else
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"link base fail ");
            
            uecom_getTime(gTimeString);      //获取基站时间
            
            mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            
            int i=0 ; //连接云服务器次数
            if(i<3&&(mflag!=0))
            {
                mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
                i++;
            }
            
            if(mflag)
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Reinit .... Fail        ");
            else
            {
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Reinit .... Success     ");
                //重新初始化成功后进行数据发送
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send .                  ");
                //结构体的地址可直接强制转为数组的地址
                mflag = uecom_send(mSendLen,mSendData);
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send ..                 ");
                if(mflag)  goto main_loop_1;    //数据发送失败，LCD显示提示
                
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send Successfully       ");
            }
            goto main_loop_2;
            main_loop_2:
            mSendFlag = 0;                       //修改发送标记
            mSendLen=0;
        }
        
        //=====================================发送流程结尾===================================================================
        
        //=====================================回发流程开始==================================================================
        //（2.4）判断是否接收到服务器发来数据，回发
        //没收到
        if (gcRecvLen<=0)   goto main_loop;   //非更新操作的数据
        //收到一个完整的帧（即gcRecvLen>0),数据在gcRecvBuf，字节数为gcRecvLen
        mRecvCount++;                         //接收次数+1
        mflag = 0xff;    
        mSendLen = 0;                         //发送数据字节数变量
        mWriteFlashFlag = 0;  //
        mLCD_RefreshFlag=1;   //LCD显示刷新标志
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Recv one frame          ");
        printf("收到一帧\n");
        printf("\r\n");
        ArrayCpy(mCmd,gcRecvBuf,2);          //命令
        ArrayCpy(mSendData,gcRecvBuf,2);     //发送数据
        //根据命令字节进行处理，为发送数据做准备----------------------
        if(mCmd[0]=='A'&&mCmd[1]=='0')       //读取flash中的所有信息
        {
            mSendLen = mFlashLen+2;
            ArrayCpy(mSendData+2,(uint8_t*)(&gFlashData),mFlashLen);
            
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='1')  //读取flash中的产品信息
        {
            mSendLen = 145;
            ArrayCpy(mSendData+2,gFlashData.equipName,mSendLen-2);
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='2')  //读取flash中的服务器信息
        {
            mSendLen = 22;
            ArrayCpy(mSendData+2,gFlashData.serverIP,mSendLen-2);
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='3')  //读取用户存入flash的信息
        {
            mSendLen = 10;
            ArrayCpy(mSendData+2,(uint8_t*)(&gFlashData.sendFrequencySec),mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='0')   //更改flash中的所有信息
        {
            ArrayCpy((uint8_t *)(gFlashData.equipName),(uint8_t*)&(gcRecvBuf[2]),mFlashLen);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='1')   //更改flash中的产品信息
        {
            ArrayCpy((uint8_t *)(gFlashData.equipName),(uint8_t*)&(gcRecvBuf[2]),124);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='2')   //更改flash中的服务器信息
        {
            ArrayCpy((uint8_t *)(gFlashData.serverIP),(uint8_t*)&(gcRecvBuf[2]),30);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='3')   //更改用户存入flash的信息
        {
            ArrayCpy((uint8_t *)(&gFlashData.sendFrequencySec),(uint8_t*)&(gcRecvBuf[2]),8);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='U'&&mCmd[1]=='0')   //获取“U0”命令要发送的数据
        {
            ArrayCpy(gFlashData.frameCmd,mCmd,2);
            if(gcRecvLen == mUserLen)         //若为整帧数据
            {
                mWriteFlashFlag  =1;
                ArrayCpy((uint8_t*)(&gUserData),gcRecvBuf,mUserLen);
                ArrayCpy(gFlashData.equipName,gUserData.equipName,30);
                ArrayCpy(gFlashData.equipID,gUserData.equipID,20);
                ArrayCpy(gFlashData.equipType,gUserData.equipType,20);
                ArrayCpy(gFlashData.vendor,gUserData.vendor,30);
                ArrayCpy(gFlashData.userName,gUserData.userName,20);
                ArrayCpy(gFlashData.phone,gUserData.phone,11);
                ArrayCpy(gFlashData.serverIP,gUserData.serverIP,15);
                ArrayCpy(gFlashData.serverPort,gUserData.serverPort,5);				
                gFlashData.sendFrequencySec = gUserData.sendFrequencySec;
                gFlashData.resetCount = gUserData.resetCount;
                ArrayCpy(gFlashData.frameCmd,gUserData.cmd,2);
                //【画瓢处2】-执行操作
            }
        }
        else if(mCmd[0]=='U'&&mCmd[1]=='1')  //获取“U1”命令要发送的数据
        {
            ArrayCpy(gFlashData.frameCmd,mCmd,2);
            
            if(gcRecvLen == 59)
            {
                ArrayCpy(gUserData.cmd,gcRecvBuf,2);
                ArrayCpy(gUserData.IMSI,gcRecvBuf+2,15);
                ArrayCpy((uint8_t *)&gUserData.currentTime,gcRecvBuf+17,8);
                ArrayCpy((uint8_t *)&gUserData.mcuTemp,gcRecvBuf+25,4);
                ArrayCpy((uint8_t *)&gUserData.signalPower,gcRecvBuf+29,1);
                ArrayCpy((uint8_t *)&gUserData.bright,gcRecvBuf+30,2);
                ArrayCpy((uint8_t *)&gUserData.touchNum,gcRecvBuf+32,2);
                ArrayCpy(gUserData.lbs_location,gcRecvBuf+34,25);
            }
        }
        
        if (mSendLen>0)                      //若有需要发送的数据
        {
            mflag = uecom_send(mSendLen,mSendData); //数据发送
        }
        gcRecvLen = 0;                    //接收数据长度清零，表明已经读取
        if(mflag==0)
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Reply Successfully      ");
        else if(mflag == 0xff)
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Recv Successfully       ");
        else
        LCD_ShowString(6,300,BLUE,GRAY,"Send Error:Send Data Not OK ");
        //判断是否需要写flash
        if(mWriteFlashFlag == 1)
        {
            flash_read_logic((uint8_t*)gcRecvBuf,(MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE);
            flash_erase(MCU_SECTOR_NUM-1);
            ArrayCpy(((uint8_t*)gcRecvBuf+28),(uint8_t*)(&gFlashData),mFlashLen);
            flash_write((MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE,(uint8_t*)gcRecvBuf);
            
            mWriteFlashFlag = 0;
        }
        //
        if (mLCD_RefreshFlag==1) 
        {
            LCD_Showfirst();                //更新LCD上的显示
            mLCD_RefreshFlag=0;
            //补充显示显示设备的IMSI号、基站位置信息、接收次数
            uecom_modelInfo(mRetdata);      //获取通信模组信息
            LCD_ShowString(60,85,BLUE,GRAY,(char *)mRetdata+20);
            LCD_ShowString(60,131,BLUE,GRAY,(char *)mLBS);  //基站位置
            LCD_ShowString(90,251,BLUE,GRAY,(char *)IntConvertToStr(mRecvCount,mString));
        }
    }
    //图形化编程之主循环流程扫描分支添加处【Graphic12】*/
    
}


//======以下为主函数调用的子函数======
//=====================================================================
//函数名称：showState
//函数返回：无
//参数说明：无
//功能概要：测试功能构件函数
//=====================================================================
void Test_9_PeripheralUnit()
{	
	uint8_t mString[30];
    //加速度传感器使用的变量
    uint8_t xyzData[6];         //x、y、z轴倾角，均占两个字节
    uint16_t xdata,ydata,zdata; //x轴倾角
    uint8_t checkdata;          //ADLX345的验证数据,正确接收为0xe5
    //（1）测试彩灯
    //一种彩灯颜色占3个字节，按grb顺序
    uint8_t grbw[12]={0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
    uint8_t rwgb[12]={0x00,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF};
    uint8_t black[12]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static char OutputCtl=0;
    OutputCtl++;

    
    if (OutputCtl>=4) OutputCtl=1;
    WS_Init(COLORLIGHT);
    //(1)彩灯测试数据
    switch (OutputCtl)
    {
        case 1:
           printf("点亮彩灯\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,grbw,4);
           break;
       case 2:
           printf("熄灭彩灯\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,black,4);
           break;
      case 3:
           printf("改变彩灯颜色\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,rwgb,4);
           break;
      default:
	       break;
    }
     
  
    //（2）蜂鸣器测试数据
     switch (OutputCtl)
    {
        case 1:
           printf("蜂鸣器发出声音\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,1);
           break;
       case 2:
           printf("蜂鸣器停止发出声音\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,0);
           break;
      case 3:
           printf("蜂鸣器发出声音\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,1);
           break;
      default:
	       break;
    }
  
    //（3）振动马达测试
    switch (OutputCtl)
    {
        case 1:
           printf("马达开始振动\r\n\r\n");
           gpio_init(MOTOR,GPIO_OUTPUT,1);
           break;
       case 2:
           printf("马达停止振动\r\n\r\n");
           gpio_init(MOTOR,GPIO_OUTPUT,0);
           break;
      case 3:
           printf("马达再次振动\n\0");
           gpio_init(MOTOR,GPIO_OUTPUT,1);
           break;
      default:
	       break;
    }
  
    
    //(4)数码管测试数据
    TM1637_Init(TM1637_CLK,TM1637_DIO);
    switch (OutputCtl)
    {
        case 1:
            printf("显示1234\r\n\r\n");
    TM1637_Display(1,1,2,1,3,1,4,1);
           
            break;
       case 2:
           printf("显示4321\r\n\r\n");
    TM1637_Display(4,1,3,1,2,1,1,1);
           break;
      case 3:
           printf("显示8888\r\n\r\n");
           TM1637_Display(8,1,8,1,8,1,8,1);
           break;
      default:
	       break;
    }
 
     
    //(5)红外寻迹测试数据
    gpio_init(RAY_RIGHT,GPIO_INPUT,0); 	//初始化红外循迹传感器两个引脚 设置为低电平输入
    gpio_init(RAY_LEFT,GPIO_INPUT,0); 
    gpio_pull(RAY_RIGHT,0);
    gpio_pull(RAY_LEFT,0);
    
    if (gpio_get(RAY_LEFT))
    {
    	printf("左侧红外：有物体\r\n\r\n");
    	LCD_ShowString(6,253,BLACK,GRAY,"[RAY-L]                    ");
    	}
    else
    {
    	printf("左侧红外：无物体\r\n\r\n");
 
    	}
    	
    if (gpio_get(RAY_RIGHT))
    {
    	printf("右侧红外：有物体\r\n\r\n");
    	LCD_ShowString(66,253,BLACK,GRAY,"[RAY-R]                   ");
    	}
    else
    {
    	printf("右侧红外：无物体\r\n\r\n");

    	}
    	

//    (6)人体红外测试数据
    gpio_init(RAY_HUMAN,GPIO_INPUT,0);		//初始化红灯和人体红外传感器模块
    gpio_pull(RAY_HUMAN,0);
 
    if (gpio_get(RAY_HUMAN))
    {
    	printf("红外：监测有人\r\n\r\n");
    	LCD_ShowString(126,253,BLACK,GRAY,"[RAY]                      ");
    	}
    else
    {
    	printf("红外：监测无人\r\n\r\n");

    	}
  
    //(7)Button测试数据
      gpio_init(Button1,GPIO_OUTPUT,0);
      gpio_init(Button3,GPIO_INPUT,0); 
      gpio_pull(Button3,1);
      if (gpio_get(Button3)==0) 
      {
      printf("S301-------------\r\n\r\n");
      LCD_ShowString(6,253,BLACK,GRAY,"[S301]                     ");
       goto Test_9_PeripheralUnit_1;
      
      }
      gpio_init(Button4,GPIO_INPUT,0); 
      gpio_pull(Button4,1);
      if (gpio_get(Button4)==0) 
      {
      printf("S302-------------\r\n\r\n");
      LCD_ShowString(6,253,BLACK,GRAY,"[S302]                     ");
      goto Test_9_PeripheralUnit_1;
      }
      
      
      gpio_init(Button3,GPIO_OUTPUT,0);
      gpio_init(Button2,GPIO_INPUT,0); 
      gpio_pull(Button2,1);
      if (gpio_get(Button2)==0) 
      {
      printf("S303-------------\r\n\r\n");
      LCD_ShowString(6,253,BLACK,GRAY,"[S303]                     ");
      goto Test_9_PeripheralUnit_1;
      }
      
      gpio_init(Button4,GPIO_OUTPUT,0);
      gpio_init(Button2,GPIO_INPUT,0); 
      gpio_pull(Button2,1);
      if (gpio_get(Button2)==0) 
      {
      printf("S304-------------\r\n\r\n");
      LCD_ShowString(6,253,BLACK,GRAY,"[S304]                     ");
      goto Test_9_PeripheralUnit_1;
      }
      
 Test_9_PeripheralUnit_1:     
   
    
    //(8)声音传感器测试数据
    
    adc_init(ADCSound,16); //初始化ADC，ADC引脚为GEC_48（PTB_NUM|1）,采样精度16
    printf("采集声音AD值为：%d\n",adc_read(ADCSound));//输出声音传感器ADC值
    LCD_ShowString(186,253,BLACK,GRAY,(char *)"       ");
    LCD_ShowString(186,253,BLACK,GRAY,(char *)IntConvertToStr(adc_read(ADCSound),mString));
    
    //(9)加速度传感器测试数据
      
    adlx345_init(i2cAcceleration,0x0B,0x08,0x08,0x80,0x00,0x00,0x05);//初始化ADLX345(J2端口) 
    adlx345_read1(0x00,&checkdata);    //读取adxl345校验数据
    adlx345_init(0,0x0B,0x08,0x08,0x80,0x00,0x00,0x05);//初始化ADLX345(J2端口)
    adlx345_read1(0x00,&checkdata);    //读取adxl345校验数据
    main_Dly_ms(5);
    adlx345_readN(0x32,xyzData,6);		   //读倾角传感器数值
    xdata = (xyzData[1]<<8)+xyzData[0];    //x方向倾角
    ydata = (xyzData[3]<<8)+xyzData[2];    //y方向倾角
    zdata = (xyzData[5]<<8)+xyzData[4];    //z方向倾角
    printf("输出x方向倾角:%d\r\n\r\n",xdata);             //输出x方向倾角
    printf("输出y方向倾角:%d\r\n\r\n",ydata);             //输出y方向倾角
    printf("输出z方向倾角:%d\n\r\n\r\n",zdata);           //输出z方向倾角
    LCD_ShowString(90,278,BLACK,GRAY,(char *)IntConvertToStr(xdata,mString));
    LCD_ShowString(140,278,BLACK,GRAY,(char *)IntConvertToStr(ydata,mString));
    LCD_ShowString(190,278,BLACK,GRAY,(char *)IntConvertToStr(zdata,mString));
   
}
//=====================================================================
//函数名称：userData_init
//函数返回：无
//参数说明：data：需要初始化的结构体数据
//功能概要：初始化用户帧结构体data
//=====================================================================
void userData_init(UserData *data)            //初始化用户帧结构体
{
    uint8_t mString[10];
    ArrayCpy(data->cmd,(uint8_t *)"U0",2);
    ArrayCpy(gFlashData.frameCmd,data->cmd,2);
    data->sn = 0;
    ArrayCpy(data->serverIP,gFlashData.serverIP,15);
    ArrayCpy(data->serverPort,gFlashData.serverPort,5);
    data->currentTime = gFlashData.productTime;
    data->resetCount = gFlashData.resetCount;
    data->sendFrequencySec = gFlashData.sendFrequencySec;
    ArrayCpy(data->userName,gFlashData.userName,20);
    //[2018.8.18] 发送的软件版本取BIOS
    uecom_version(mString);//??????????????????????
    ArrayCpy(data->softVer,mString,4);
    ArrayCpy(data->equipName,gFlashData.equipName,30);
    ArrayCpy(data->equipID,gFlashData.equipID,20);
    ArrayCpy(data->equipType,gFlashData.equipType,20);
    ArrayCpy(data->vendor,gFlashData.vendor,30);
    ArrayCpy(data->phone,gFlashData.phone,11);
    data->touchNum = 0;
    ArrayCpy(data->cmd,gFlashData.frameCmd,2);
    //【画瓢处2】-初始化数据
    
}

//=====================================================================
//函数名称：userData_get
//函数返回：无
//参数说明：data：需要赋值的结构体数据
//功能概要：给用户帧结构体data赋值
//=====================================================================
void userData_get(UserData *data)             //给用户帧结构体data赋值
{
    uint16_t brightAD,mcu_temp_AD;
    char mStr[6];
    float mcu_temp;
    
    static uint32_t sn = 0;
    data->sn = sn++;    
    //获取mcu温度
    mcu_temp_AD = adc_read(AD_MCU_TEMP);       //读取mcu温度通道AD值
    mcu_temp=TempTrans(mcu_temp_AD);           //温度回归
    LCD_ShowString(150,105,BLUE,GRAY,(char *)FloatConvertToStr(mcu_temp,1,mStr));  //LCD上显示芯片温度
    
    data->mcuTemp =(int32_t) (mcu_temp*10);
    printf("芯片温度=%6.2f\r\n",mcu_temp);
    printf("\r\n");
    
    //获取光照强度
    brightAD = adc_read(AD_LIGHT);
    data->bright = brightAD;
    main_TimeStamp();   //给gUserData.currentTime赋值
    
    
    
    //【画瓢处1】-数据获取
    
    
}

//=====================================================================
//函数名称：LCD_Showfirst
//函数返回：无
//参数说明：无
//功能概要：初始化LCD上电显示的内容
//=====================================================================
void LCD_Showfirst()
{
    uint8_t temp[30] = {0};
    //uint8_t tempertature[6];
    //（1）设置全局底色为灰色
    LCD_DrawSurface(0,0,240,320,GRAY);    //240*320像素LCD
    //（2）设置第一区（标题区）
    LCD_aotu(2,2,238,38,1);               //LCD指定区域凸起
    LCD_ShowString(66,15,RED,GRAY,"金葫芦IoT-GEC"); //红字
    wdog_feed();
    //（3）设置第二区（与通信无关区）
    LCD_aotu(2,43,238,123,0);             //LCD指定区域凹下
    //显示型号
    LCD_ShowString(6,45,BLACK,GRAY,"[Type]                     ");
    uecom_typeGet(temp);
    temp[20]=0;
    LCD_ShowString(60,45,BLUE,GRAY,(char *)temp);
    //显示BIOS软件版本
    wdog_feed();
    LCD_ShowString(6,65,BLACK,GRAY,"[BIOS]                      ");
    uecom_version(temp);   //取uecom版本号（作为BIOS版本号）
    LCD_ShowString(58,65,BLUE,GRAY,(char *)temp);   //显示BIOS软件版本
    //显示user软件版本
    LCD_ShowString(120,65,BLACK,GRAY,"[USER]        ");
    ArrayCpy(temp,gFlashData.softVer,4);
    temp[5]=0;
    LCD_ShowString(172,65,BLUE,GRAY,(char *)temp);
    //显示IMSI提示
    LCD_ShowString(6,85,BLACK,GRAY,"[IMSI]                      ");
    //显示MCU温度
    LCD_ShowString(6,105,BLACK,GRAY,"[MCU_temperature]           ");
    
    //（4）设置第三区（与通信相关）
    LCD_aotu(2,127,238,228,1);            //LCD指定区域凸起
    LCD_ShowString(6,131,BLACK,GRAY,"[LBS]                      ");
    LCD_ShowString(6,149,BLACK,GRAY,"[Signal strength(%)]       ");
    //显示IP:PT (IP:PORT)
    LCD_ShowString(6,169,BLACK,GRAY,"[IP:PT]                    ");
    ArrayCpy(temp,gFlashData.serverIP,15);   
    temp[15]=0;
    LCD_ShowString(65,169,BLUE,GRAY,(char *)temp);
    LCD_ShowString(185,169,BLUE,GRAY,":");
    ArrayCpy(temp,gFlashData.serverPort,5);
    temp[5]=0;
    LCD_ShowString(195,169,BLUE,GRAY,(char *)temp);
    //显示发送频率
    LCD_ShowString(6,189,BLACK,GRAY,"TSI:       Freq(s):        ");
    LCD_ShowString(180,189,BLUE,GRAY,"      ");
    LCD_ShowString(180,189,BLUE,GRAY,
    (char *)IntConvertToStr(gFlashData.sendFrequencySec,temp));
    LCD_ShowString(40,189,BLUE,GRAY,"0"); //显示TSI次数初值0
    LCD_ShowString(6,209,BLACK,GRAY,"Time:                      ");
    LCD_ShowString(49,209,BLUE,GRAY,"0000-00-00 00:00:00" );
    LCD_aotu(2,232,238,271,0);            //LCD指定区域凹下
    LCD_ShowString(6,235,BLACK,GRAY,"5-year flow fee(2020-2024) ");
    
    LCD_ShowString(6,253,BLACK,GRAY,"RecvCount:                 ");
    //（5）设置第四区（运行状态显示区）
    LCD_aotu(2,275,238,317,1);            //LCD指定区域凸起
    LCD_ShowString(6,278,BLACK,GRAY,"Run State:                 ");
}
//图形化编程之main文件子函数添加处【Graphic13】


//=====================================================================
//函数名称：ArrayCpy
//函数返回：无
//参数说明：dest：复制后存放的数组；source：被复制的数组；len:复制的长度
//功能概要：从源数组复制指定长度的内容到目标数组
//=====================================================================
void ArrayCpy(uint8_t * dest,uint8_t*source,uint16_t len)
{
    for(uint16_t r=0;r<len;r++)  dest[r]=source[r];
}

void main_Dly_ms(uint32_t ms)
{
    for(uint32_t i= 0; i < (6000*ms); i++) __asm("nop");
}


void  main_TimeStamp()
{
    //年月日时分秒 十进制
    uint16_t tyear,tmonth,tday,thour,tmin,tsec;
    uint16_t i;
    uint64_t timestamp;	    //时间戳
    uint8_t st[20];
    //每月份之前的天数，闰年另计
    uint16_t MonthDay[12]={31,59,90,120,151,181,212,243,273,304,334,365};
    timestamp=0;
    //将年份化为十进制
    for (i=0;i<=3;i++) st[i]=gTimeString[i]-0x30;   
    tyear=st[0]*1000+st[1]*100+st[2]*10+st[3];
    //将月份化为十进制
    for (i=5;i<=6;i++) st[i]=gTimeString[i]-0x30;  
    tmonth=st[5]*10+st[6];
    //将天化为十进制
    for (i=8;i<=9;i++) st[i]=gTimeString[i]-0x30;  
    tday=st[8]*10+st[9];
    //将时化为十进制
    for (i=11;i<=12;i++) st[i]=gTimeString[i]-0x30;  
    thour=st[11]*10+st[12];
    //将分化为十进制
    for (i=14;i<=15;i++) st[i]=gTimeString[i]-0x30;  
    tmin=st[14]*10+st[15];
    //将秒化为十进制
    for (i=17;i<=18;i++) st[i]=gTimeString[i]-0x30;  
    tsec=st[17]*10+st[18];
    //计算之前年份的秒数
    for(i=1970;i<tyear;i++)
    {
        if(i%4)
        timestamp+=31536000;
        
        else
        timestamp+=31622400;
    }
    //计算之前月份的秒数
    if(tmonth>12) return;
    if(tmonth>2 && tyear%4)
    timestamp+=MonthDay[tmonth-2]*24*3600;
    else if(tmonth>2 && (tyear%4==0))
    timestamp+=(MonthDay[tmonth-2]+1)*24*3600;
    else if(tmonth==2)
    timestamp+=31*24*3600;
    else
    timestamp+=0;
    //计算当前天的秒数
    if(tday>1) timestamp+=(tday-1)*24*3600;
    timestamp+=(thour*3600)+(tmin*60)+tsec;
    gUserData.currentTime=timestamp;
    
}


/*
知识要素：
1.main.c是一个具体的实例，执行相关程序流程不会涉及任何环境，芯片问题。
该文件所有代码均不会涉及具体的硬件和环境，它通过调用相关构件来实现与硬件
系统的交互。
2.本文件共有两类代码，一类为【根据实际需要增删】，此类代码根据具体
项目需求进行更改；另一类为【不变】，此类代码与具体项目无关，是通用的，
无需根据具体项目进行更改。
3.本文件对宏GLOBLE_VAR进行了定义， 所以在包含"includes.h"头文件时
会定义全局变量，在其他文件中包含"includes.h"头文件时，仅声明该头文件中
的全局变量。
*/



