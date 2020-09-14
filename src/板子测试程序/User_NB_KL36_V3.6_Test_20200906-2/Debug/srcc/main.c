//======================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ���մ�arm�������ģ�sumcu.suda.edu.cn��
//�汾���£�2017.08:1.0, 2019.1:A.10
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//======================================================================
#define GLOBLE_VAR
#include "includes.h"      //������ͷ�ļ�

//������ʵ����Ҫ��ɾ����ʼ�������������¼126������ֵ


//----------------------------------------------------------------------

//----------------------------------------------------------------------
//������ʵ����Ҫ��ɾ������ʹ�õ����ڲ�����
//main.cʹ�õ��ڲ�����������
void userData_init(UserData *data);   //��ʼ���û�֡�ṹ��data
void userData_get(UserData *data);    //���û�֡�ṹ��data��ֵ
void LCD_Showfirst();                 //��ʼ��LCD����ʾ����Ҫ�Ǳ�ǩ
void ArrayCpy(uint8_t * dest,uint8_t*source,uint16_t len);
void main_Dly_ms(uint32_t ms);
void main_TimeStamp();
void Test_9_PeripheralUnit();
//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1���������֣���ͷ��======
    
    //��1.1������main����ʹ�õľֲ�����
    uint16_t mi,mj;                  //������ʹ�õ���16λѭ������
    int mTsi;                       //��¼��ǰ������������
    uint8_t  mSendFlag;              //�������ݱ�־λ;1:����;0:������
    uint8_t  mRetdata[100];          //���uecom��ʼ�����صĽ��
    uint16_t mUserLen;               //��Ҫ���͵��ֽڳ���
    uint16_t mFlashLen;              //��Ҫ�洢flash���ֽڳ���
    uint64_t mRecvCount;             //�յ��Ĵ���
    uint8_t mString[30];             //����ת�ı�ʹ�õ���ʱ����
    uint8_t mCmd[2];                 //�洢����
    uint8_t mflag;                   //��ѭ��ʹ�õı�־�жϱ���
    uint8_t mWriteFlashFlag;         //�����Ƿ���Ҫ������д��flash
    uint8_t mLinkCount;              //������վ���Ӵ���
    int mLCD_RefreshFlag;            //LCD��ʾˢ�±�־
    uint8_t  mSendData[1000];        //���������ݻ�����
    uint16_t mSendLen;               //���������ݵĳ���
    uint8_t  mLBS[30];               //�洢LBS��λ��Ϣ
    uint8_t  mSec;                   //�������ʱ������λ����
    uint8_t  mSecOld;                //�������ʱ������λ����
    uint16_t  mSendFreq;  
    uint16_t  mSendFreqOld;  
    uint8_t  mUE_infor[40];           //�洢UE��Ϣ
    uint8_t mUpdateUE_flag;           //��־λ����Ҫ����ģ����Ϣ
    
    //��1.2�������䡿�����ж�
    DISABLE_INTERRUPTS;
    
    //��1.3�������������ʱ��������ֵ     
    mTsi=0;                            //��մ�����������
    mSendFlag = 1;                     //Ĭ���ն˷�������
    mWriteFlashFlag = 0;               //Ĭ�ϲ�д��flash
    mLinkCount=0;                      //��վ���Ӵ���=0
    mUserLen = sizeof(UserData);      //�����Ҫ���͵��ֽڳ���
    mFlashLen = sizeof(FlashData);    //��ô���flash���ֽڳ���
    mRecvCount = 0;                    //��ս��մ���
    mLCD_RefreshFlag=0;                //LCD��ʾˢ�±�־
    mSendLen=0;
    mSec=0;
    mSecOld=0;
    mSendFreq=0;  
    mSendFreqOld=0; 
    mUpdateUE_flag=1;                  //��־λ����Ҫ����ģ����Ϣ
    
    //��1.4����ȫ�ֱ�������ֵ
    ArrayCpy(gTimeString,(uint8_t *)"0000-00-00 00:00:00\0",20);
    gCount=0;
    gUserData.touchNum=0;
    
    //��1.5���û�����ģ���ʼ��
    uecom_power(UECOM_OFF);
    gpio_init(LIGHT_RED,GPIO_OUTPUT,LIGHT_ON);    //��ʼ�����
    gpio_init(GPIO_TSI,GPIO_INPUT,1);             //GPIOģ�ⴥ��
    uart_init(UART_UE,115200);                    //�û����ڳ�ʼ��
    timer_init(TIMER_USER,20);                    //�û���ʱ����ʼ��
    flash_init();                                 //��ʼ��flash
    LCD_Init();                                   //��ʼ��LCD   
    adc_init(AD_LIGHT,0);                 		   //��ʼ��AD����ģ��
    adc_init(AD_MCU_TEMP,0);					   //��ʼ��ADоƬ�¶�ģ��
    
    //����ư��1��-��ʼ��
    
    //��1.6��ʹ��ģ���ж�
    timer_enable_int(TIMER_USER);                //ʹ��LPTMR�ж�
    
    //��1.7�������䡿�����ж�
    ENABLE_INTERRUPTS;

    
    //��1.6��ʹ��ģ���ж�
    timer_enable_int(TIMER_USER);    //������ʱ���ж�
    
    //��1.8����ͨ��ģ�鹩��
    uecom_power(UECOM_ON);            
    printf("AHL-IoT-GEC start...  \r\n");
    
    //��1.9��������ʵ����Ҫ��ɾ�� ��ѭ��ǰ�ĳ�ʼ������
    //��Flash���һ������д�û�����   
    printf("flash_erase and write...        \r\n");
    flash_erase(MCU_SECTOR_NUM-1);    //�������һ������
    //�����һ������д����
    flash_write((MCU_SECTOR_NUM-1),28,sizeof(FlashData),
    (uint8_t *)flashInit);
    printf("flash_erase and write...OK     \r\n");
    //��1.9.1����ȡflash�е�������Ϣ��gFlashData����ʼ���û�֡����gUserData
    //��ȡFlash��126�����Ĳ�����Ϣ��gFlashData��   
    flash_read_logic((uint8_t*)(&gFlashData),
    (MCU_SECTOR_NUM-1),28,sizeof(FlashData));
    userData_init(&gUserData);         //��ʼ���û�֡�ṹ��gUserData
    LCD_Showfirst();                   //LCD��ʾ��ʼ����
    //��1.9.2���жϸ�λ״̬��������λ״̬���ݴ洢��flash�У�ע�ⲻӰ��ԭ�����ݣ�
    if (IS_POWERON_RESET)               //�临λ������
    gFlashData.resetCount = 0;
    else                                //�ȸ�λ�����1 
    {
        gFlashData.resetCount++;
        flash_read_logic((uint8_t*)gcRecvBuf,(MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE);
        flash_erase(MCU_SECTOR_NUM-1);
        ArrayCpy(((uint8_t*)gcRecvBuf+166),(uint8_t*)(gFlashData.resetCount),4);
        flash_write((MCU_SECTOR_NUM-1),0,MCU_SECTORSIZE,(uint8_t*)gcRecvBuf);
    }
    
    //��1.9.3����ʼ��ͨ��ģ�飬����LCD����ʾ��ʼ������
    //LCD��һ�������ʾ28���ֽ�
    printf("���«��ʾ������ͨ��ģ���ʼ��... \r\n");
    for(;;)                           //��ʼ��ͨ��ģ��ѭ��
    {
        //ͨ��ģ���ϵ粢�ȴ���ʱ�ȴ�Լ12��
        uecom_power(UECOM_ON);        //��ͨ��ģ�鹩��
        main_Dly_ms(3000);            //��ʱ6s
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init . uecom_power      ");
        printf("���«��ʾ��AHL Init . ��ͨ��ģ�鹩�� \r\n");
        main_Dly_ms(3000);            //��ʱ6s
        //ͨ��ģ���ʼ�������������ͽ����������ӹ���
        //��ʼ��ͨ��ģ��
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ..  uecom_init     ");
        printf("���«��ʾ��AHL Init .. ��ʼ��ͨ��ģ�� \r\n");
        mflag =uecom_init();
        if(mflag)
        {
            if (mflag==1)
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init .. AT Error        ");
            printf("���«��ʾ��AHL Init .. ATָ�����\r\n");
            if (mflag==2)
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init .. SIM Error       ");
            printf("���«��ʾ��AHL Init .. ��SIM������\r\n");
            uecom_power(UECOM_OFF);  //ͨ��ģ������
            continue;
        }
        //��ʾ�豸��IMSI��
        uecom_modelInfo(mRetdata);    //��ȡͨ��ģ����Ϣ
        LCD_ShowString(60,85,BLUE,GRAY,(char *)mRetdata+20);
        //���վ��������
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ... uecom_linkBase ");
        printf("AHL Init ...  ���վ��������   \r\n");
        mflag =uecom_linkBase();
        while(mflag)
        {
            mflag =uecom_linkBase();
            mLinkCount++; //���Ӵ���+1
            LCD_ShowString(6,300,BLUE,GRAY,(char *)IntConvertToStr(mLinkCount,mString));
            printf("���վ�������Ӵ�����%d\r\n",mLinkCount);
            if(mLinkCount>10)                //���վ���Ӵ��� 
            {
                mLinkCount=0;
                uecom_power(UECOM_OFF);    //ͨ��ģ������
                main_Dly_ms(2000);      //��ʱ
                uecom_power(UECOM_REBOOT); //ͨ��ģ������
                main_Dly_ms(2000);      //��ʱ
                mflag =uecom_init();
                break;
            }
        }
        
        if(mflag)
        {
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ...link base Error ");
            uecom_power(UECOM_OFF);    //ͨ��ģ������
            main_Dly_ms(1000);      //��ʱ
            uecom_power(UECOM_REBOOT); //ͨ��ģ������
            main_Dly_ms(1000);      //��ʱ
            continue;
            
        }
        //��ʾ�ź�ǿ�Ⱥ�С���ţ���վ�ţ�λ����Ϣ LBS
        for(mi=0;mi<5;mi++)
        {
            mflag = uecom_baseInfo(mRetdata);
            if(mflag)
            {
                main_Dly_ms(1000);
                continue;
            }
            ArrayCpy(mLBS,mRetdata+1,19);  //λ����Ϣ������mLBS�б���
            LCD_ShowString(60,131,BLUE,GRAY,(char *)mRetdata+1);  //��վλ��
            
            LCD_ShowString(170,150,BLUE,GRAY,
            (char *)IntConvertToStr(mRetdata[0],(uint8_t*)mString));
        }
        //���������������
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ....uecom_linkCS   ");
        printf("AHL Init .... ����ƽ̨��������   \r\n");
        mflag =uecom_linkCS((uint8_t*)gFlashData.serverIP,(uint8_t*)gFlashData.serverPort);
        if(mflag)
        {
            LCD_ShowString(6,300,BLUE,GRAY,"AHL....Link CS-Monitor Error");
            uecom_power(UECOM_REBOOT);  //ͨ��ģ������
            continue;
        }
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Init ..... Successfully ");
        printf("AHL Init ..... Successfully \r\n");
        break;
    }
    uecom_getTime(gTimeString);      //��ȡ��վʱ��
    
    LCD_ShowString(49,209,BLUE,GRAY,(char *)gTimeString);     	//��LCD����ʾ��ǰʱ��
    printf("NB-IoT communication OK!  \n");
    //��1���������֣���β��======
    printf("Go to Main Loop\r\n");
    //��2����ѭ�����֣���ͷ��======
    
    for(;;)
    {
        main_loop: 
        //ȡ���λ��ASCII�룬�����ж�
        
        
        
        mSec=gTimeString[18]; 
        if (mSec==mSecOld) continue;   //һ��δ�����ص�ѭ����ʼ��������ѭ��
        Test_9_PeripheralUnit();
         
        //��2.1��һ�뵽��֮����еĲ���
        mSecOld=mSec;                  //mSecOld��������
        //��LCD����ʾ��ǰʱ�䡢���ƺ����˸����PC�����ʱ��
        LCD_ShowString(49,209,BLUE,GRAY,(char *)gTimeString);    //��LCD����ʾ��ǰʱ��
        gpio_reverse(LIGHT_RED);    //���ÿ����˸һ��
        printf("��ǰʱ��Ϊ��%s\r\n",gTimeString);
        
        //��2.1.3���ж��Ƿ񵽴﷢�����ݵ�ʱ��
        mSendFreq++;
        if (mSendFreq-mSendFreqOld>=gFlashData.sendFrequencySec) 
        {
            mSendFreqOld=mSendFreq; 
            mSendFlag = 1;                         //���ͱ�־��1��
            printf("%d ��ʱ�䵽������һ֡���ݣ�\r\n",(int)gFlashData.sendFrequencySec);
            printf("\r\n");
        }
        
        mj=0;
        for (mi=0;mi<3000;mi++) 
        mj=mj+gpio_get(GPIO_TSI);
        if (mj<2000) 
        {
            mTsi++;
            printf("�������� = %d\r\n", mTsi);
            LCD_ShowString(40,189,BLUE,GRAY,(char *)IntConvertToStr(mTsi,mString));    //LCD����ʾTSI��������
            
            printf("\r\n");
            if ((mTsi-gUserData.touchNum)>=3)
            {
                mSendFlag = 1;
                printf("�������Σ�����һ֡���ݣ�\r\n");
                printf("\r\n");
                gUserData.touchNum=mTsi;
                printf("�������� ============= %d\n", mTsi);
                printf("\r\n");
            } 
        }    
        
        //��2.3������Ҫִ�з������ݲ�������������в���
        if(mSendFlag == 1)
        {	   
            //��2.3.1�������û�����Ϊ��������
            userData_get(&gUserData);      //���û�֡�ṹ��gUserData��ֵ
            if(mUpdateUE_flag==1)
            {
                mUpdateUE_flag=0;
                uecom_modelInfo(mUE_infor);   //����UE��Ϣ
                ArrayCpy(gUserData.IMEI,mUE_infor,15);
                ArrayCpy(gUserData.IMSI,mUE_infor+20,15);
                uecom_baseInfo(mUE_infor);
                gUserData.signalPower = mUE_infor[0];
                ArrayCpy(gUserData.lbs_location,mUE_infor+1,19);
            }
            
            //��2.3.2��������������Ҫ���͵�����
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
                //��20200816��
                main_TimeStamp();   //��gUserData.currentTime��ֵ
                
                ArrayCpy(mSendData+17,(uint8_t *)&gUserData.currentTime,8);
                
                ArrayCpy(mSendData+25,(uint8_t *)&gUserData.mcuTemp,4);
                ArrayCpy(mSendData+29,(uint8_t *)&gUserData.signalPower,1);
                ArrayCpy(mSendData+30,(uint8_t *)&gUserData.bright,2);
                ArrayCpy(mSendData+32,(uint8_t *)&gUserData.touchNum,2);
                ArrayCpy(mSendData+34,gUserData.lbs_location,25);
                //����ư��3�����ġ�U1������͵�����
            }
            //��2.3.3����ʾ�ź�ǿ��
            LCD_ShowString(170,150,BLUE,GRAY,"     ");
            LCD_ShowString(170,150,BLUE,GRAY,
            (char *)IntConvertToStr(gUserData.signalPower,(uint8_t *)mString));
            
            mLBS[20]='\0';
            gUserData.serverIP[13]='\0';
            gUserData.IMSI[15]='\0';
            printf("�ź�ǿ��:%d\n\n",gUserData.signalPower);	
            printf("LBS��:%s\r\n\n",mLBS);
            printf("���մ���:%d\n\n",mRecvCount);
            printf("IP:PT  %s : %s\n\n",gFlashData.serverIP,gFlashData.serverPort);
            printf("Frep(s):%d\n\n",gFlashData.sendFrequencySec);
            printf("IMSI:%s\n\n",gUserData.IMSI);
            
            
            
            //��2.3.4��UEģ�鷢������
            LCD_ShowString(6,300,BLUE,GRAY,"AHL Send .                  ");
            
            mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            //��20200818��  ����ǰȷ�������Ʒ������Ƿ�ɹ����Ա���ٷ���
            uint8_t temp=0 ;                 //�����Ʒ���������
            if(temp<3&&(mflag!=0))
            {
                mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
                LCD_ShowString(6,300,BLUE,GRAY,"AHL....Link CS-Monitor Error");
                uecom_power(UECOM_REBOOT);  //ͨ��ģ������
                temp++;
            }
            //��20200818��  ��������ʧ�ܣ����³�ʼ��
            if(mflag!=0)
            {
                printf("�������CS_monitorʧ�ܣ����³�ʼ��\r\n");
                
            }
            
            
            //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
            mflag = uecom_send(mSendLen,mSendData);
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Send ..                 ");
            //if(mflag)  goto main_loop_1;    //���ݷ���ʧ�ܣ�LCD��ʾ��ʾ
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Send Successfully       ");
            goto main_loop_2;
            main_loop_1:
            //���ݷ���ʧ����ʾ
            switch(mflag)
            {
                case 1:
                LCD_ShowString(6,300,BLUE,GRAY,(char *)"Send Error:Send Not Start   ");
                break;
                case 2:
                LCD_ShowString(6,300,BLUE,GRAY,(char *)"Send Error:Send Data Not OK ");
                break;
            }
            //���³�ʼ��
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Reinit .                ");
            uecom_power(UECOM_OFF);    //ͨ��ģ������
            main_Dly_ms(3000);         //��ʱ
            uecom_power(UECOM_REBOOT); //ͨ��ģ������
            main_Dly_ms(3000);         //��ʱ
            uecom_init();
            main_Dly_ms(100);
            //�������γ�ʼ��������
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"AHL Reinit ..                ");
            if(uecom_init())
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"uecom init success ");
            else
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"uecom init fail ");
            
            if(uecom_linkBase())
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"link base success ");
            else
            LCD_ShowString(6,300,BLUE,GRAY,(char *)"link base fail ");
            
            uecom_getTime(gTimeString);      //��ȡ��վʱ��
            
            mflag = uecom_linkCS(gFlashData.serverIP,gFlashData.serverPort);
            
            int i=0 ; //�����Ʒ���������
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
                //���³�ʼ���ɹ���������ݷ���
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send .                  ");
                //�ṹ��ĵ�ַ��ֱ��ǿ��תΪ����ĵ�ַ
                mflag = uecom_send(mSendLen,mSendData);
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send ..                 ");
                if(mflag)  goto main_loop_1;    //���ݷ���ʧ�ܣ�LCD��ʾ��ʾ
                
                LCD_ShowString(6,300,BLUE,GRAY,"AHL Send Successfully       ");
            }
            goto main_loop_2;
            main_loop_2:
            mSendFlag = 0;                       //�޸ķ��ͱ��
            mSendLen=0;
        }
        
        //=====================================�������̽�β===================================================================
        
        //=====================================�ط����̿�ʼ==================================================================
        //��2.4���ж��Ƿ���յ��������������ݣ��ط�
        //û�յ�
        if (gcRecvLen<=0)   goto main_loop;   //�Ǹ��²���������
        //�յ�һ��������֡����gcRecvLen>0),������gcRecvBuf���ֽ���ΪgcRecvLen
        mRecvCount++;                         //���մ���+1
        mflag = 0xff;    
        mSendLen = 0;                         //���������ֽ�������
        mWriteFlashFlag = 0;  //
        mLCD_RefreshFlag=1;   //LCD��ʾˢ�±�־
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Recv one frame          ");
        printf("�յ�һ֡\n");
        printf("\r\n");
        ArrayCpy(mCmd,gcRecvBuf,2);          //����
        ArrayCpy(mSendData,gcRecvBuf,2);     //��������
        //���������ֽڽ��д���Ϊ����������׼��----------------------
        if(mCmd[0]=='A'&&mCmd[1]=='0')       //��ȡflash�е�������Ϣ
        {
            mSendLen = mFlashLen+2;
            ArrayCpy(mSendData+2,(uint8_t*)(&gFlashData),mFlashLen);
            
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='1')  //��ȡflash�еĲ�Ʒ��Ϣ
        {
            mSendLen = 145;
            ArrayCpy(mSendData+2,gFlashData.equipName,mSendLen-2);
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='2')  //��ȡflash�еķ�������Ϣ
        {
            mSendLen = 22;
            ArrayCpy(mSendData+2,gFlashData.serverIP,mSendLen-2);
        }
        else if(mCmd[0]=='A'&&mCmd[1]=='3')  //��ȡ�û�����flash����Ϣ
        {
            mSendLen = 10;
            ArrayCpy(mSendData+2,(uint8_t*)(&gFlashData.sendFrequencySec),mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='0')   //����flash�е�������Ϣ
        {
            ArrayCpy((uint8_t *)(gFlashData.equipName),(uint8_t*)&(gcRecvBuf[2]),mFlashLen);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='1')   //����flash�еĲ�Ʒ��Ϣ
        {
            ArrayCpy((uint8_t *)(gFlashData.equipName),(uint8_t*)&(gcRecvBuf[2]),124);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='2')   //����flash�еķ�������Ϣ
        {
            ArrayCpy((uint8_t *)(gFlashData.serverIP),(uint8_t*)&(gcRecvBuf[2]),30);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='B'&&mCmd[1]=='3')   //�����û�����flash����Ϣ
        {
            ArrayCpy((uint8_t *)(&gFlashData.sendFrequencySec),(uint8_t*)&(gcRecvBuf[2]),8);
            mWriteFlashFlag = 1;
            mSendLen = 9;
            ArrayCpy((uint8_t *)mSendData+2,(uint8_t *)"success",mSendLen-2);
        }
        else if(mCmd[0]=='U'&&mCmd[1]=='0')   //��ȡ��U0������Ҫ���͵�����
        {
            ArrayCpy(gFlashData.frameCmd,mCmd,2);
            if(gcRecvLen == mUserLen)         //��Ϊ��֡����
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
                //����ư��2��-ִ�в���
            }
        }
        else if(mCmd[0]=='U'&&mCmd[1]=='1')  //��ȡ��U1������Ҫ���͵�����
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
        
        if (mSendLen>0)                      //������Ҫ���͵�����
        {
            mflag = uecom_send(mSendLen,mSendData); //���ݷ���
        }
        gcRecvLen = 0;                    //�������ݳ������㣬�����Ѿ���ȡ
        if(mflag==0)
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Reply Successfully      ");
        else if(mflag == 0xff)
        LCD_ShowString(6,300,BLUE,GRAY,"AHL Recv Successfully       ");
        else
        LCD_ShowString(6,300,BLUE,GRAY,"Send Error:Send Data Not OK ");
        //�ж��Ƿ���Ҫдflash
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
            LCD_Showfirst();                //����LCD�ϵ���ʾ
            mLCD_RefreshFlag=0;
            //������ʾ��ʾ�豸��IMSI�š���վλ����Ϣ�����մ���
            uecom_modelInfo(mRetdata);      //��ȡͨ��ģ����Ϣ
            LCD_ShowString(60,85,BLUE,GRAY,(char *)mRetdata+20);
            LCD_ShowString(60,131,BLUE,GRAY,(char *)mLBS);  //��վλ��
            LCD_ShowString(90,251,BLUE,GRAY,(char *)IntConvertToStr(mRecvCount,mString));
        }
    }
    //ͼ�λ����֮��ѭ������ɨ���֧��Ӵ���Graphic12��*/
    
}


//======����Ϊ���������õ��Ӻ���======
//=====================================================================
//�������ƣ�showState
//�������أ���
//����˵������
//���ܸ�Ҫ�����Թ��ܹ�������
//=====================================================================
void Test_9_PeripheralUnit()
{	
	uint8_t mString[30];
    //���ٶȴ�����ʹ�õı���
    uint8_t xyzData[6];         //x��y��z����ǣ���ռ�����ֽ�
    uint16_t xdata,ydata,zdata; //x�����
    uint8_t checkdata;          //ADLX345����֤����,��ȷ����Ϊ0xe5
    //��1�����Բʵ�
    //һ�ֲʵ���ɫռ3���ֽڣ���grb˳��
    uint8_t grbw[12]={0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
    uint8_t rwgb[12]={0x00,0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF};
    uint8_t black[12]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    static char OutputCtl=0;
    OutputCtl++;

    
    if (OutputCtl>=4) OutputCtl=1;
    WS_Init(COLORLIGHT);
    //(1)�ʵƲ�������
    switch (OutputCtl)
    {
        case 1:
           printf("�����ʵ�\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,grbw,4);
           break;
       case 2:
           printf("Ϩ��ʵ�\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,black,4);
           break;
      case 3:
           printf("�ı�ʵ���ɫ\r\n\r\n");
           WS_SendOnePix(COLORLIGHT,rwgb,4);
           break;
      default:
	       break;
    }
     
  
    //��2����������������
     switch (OutputCtl)
    {
        case 1:
           printf("��������������\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,1);
           break;
       case 2:
           printf("������ֹͣ��������\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,0);
           break;
      case 3:
           printf("��������������\r\n\r\n");
           gpio_init(BEEF,GPIO_OUTPUT,1);
           break;
      default:
	       break;
    }
  
    //��3����������
    switch (OutputCtl)
    {
        case 1:
           printf("��￪ʼ��\r\n\r\n");
           gpio_init(MOTOR,GPIO_OUTPUT,1);
           break;
       case 2:
           printf("���ֹͣ��\r\n\r\n");
           gpio_init(MOTOR,GPIO_OUTPUT,0);
           break;
      case 3:
           printf("����ٴ���\n\0");
           gpio_init(MOTOR,GPIO_OUTPUT,1);
           break;
      default:
	       break;
    }
  
    
    //(4)����ܲ�������
    TM1637_Init(TM1637_CLK,TM1637_DIO);
    switch (OutputCtl)
    {
        case 1:
            printf("��ʾ1234\r\n\r\n");
    TM1637_Display(1,1,2,1,3,1,4,1);
           
            break;
       case 2:
           printf("��ʾ4321\r\n\r\n");
    TM1637_Display(4,1,3,1,2,1,1,1);
           break;
      case 3:
           printf("��ʾ8888\r\n\r\n");
           TM1637_Display(8,1,8,1,8,1,8,1);
           break;
      default:
	       break;
    }
 
     
    //(5)����Ѱ����������
    gpio_init(RAY_RIGHT,GPIO_INPUT,0); 	//��ʼ������ѭ���������������� ����Ϊ�͵�ƽ����
    gpio_init(RAY_LEFT,GPIO_INPUT,0); 
    gpio_pull(RAY_RIGHT,0);
    gpio_pull(RAY_LEFT,0);
    
    if (gpio_get(RAY_LEFT))
    {
    	printf("�����⣺������\r\n\r\n");
    	LCD_ShowString(6,253,BLACK,GRAY,"[RAY-L]                    ");
    	}
    else
    {
    	printf("�����⣺������\r\n\r\n");
 
    	}
    	
    if (gpio_get(RAY_RIGHT))
    {
    	printf("�Ҳ���⣺������\r\n\r\n");
    	LCD_ShowString(66,253,BLACK,GRAY,"[RAY-R]                   ");
    	}
    else
    {
    	printf("�Ҳ���⣺������\r\n\r\n");

    	}
    	

//    (6)��������������
    gpio_init(RAY_HUMAN,GPIO_INPUT,0);		//��ʼ����ƺ�������⴫����ģ��
    gpio_pull(RAY_HUMAN,0);
 
    if (gpio_get(RAY_HUMAN))
    {
    	printf("���⣺�������\r\n\r\n");
    	LCD_ShowString(126,253,BLACK,GRAY,"[RAY]                      ");
    	}
    else
    {
    	printf("���⣺�������\r\n\r\n");

    	}
  
    //(7)Button��������
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
   
    
    //(8)������������������
    
    adc_init(ADCSound,16); //��ʼ��ADC��ADC����ΪGEC_48��PTB_NUM|1��,��������16
    printf("�ɼ�����ADֵΪ��%d\n",adc_read(ADCSound));//�������������ADCֵ
    LCD_ShowString(186,253,BLACK,GRAY,(char *)"       ");
    LCD_ShowString(186,253,BLACK,GRAY,(char *)IntConvertToStr(adc_read(ADCSound),mString));
    
    //(9)���ٶȴ�������������
      
    adlx345_init(i2cAcceleration,0x0B,0x08,0x08,0x80,0x00,0x00,0x05);//��ʼ��ADLX345(J2�˿�) 
    adlx345_read1(0x00,&checkdata);    //��ȡadxl345У������
    adlx345_init(0,0x0B,0x08,0x08,0x80,0x00,0x00,0x05);//��ʼ��ADLX345(J2�˿�)
    adlx345_read1(0x00,&checkdata);    //��ȡadxl345У������
    main_Dly_ms(5);
    adlx345_readN(0x32,xyzData,6);		   //����Ǵ�������ֵ
    xdata = (xyzData[1]<<8)+xyzData[0];    //x�������
    ydata = (xyzData[3]<<8)+xyzData[2];    //y�������
    zdata = (xyzData[5]<<8)+xyzData[4];    //z�������
    printf("���x�������:%d\r\n\r\n",xdata);             //���x�������
    printf("���y�������:%d\r\n\r\n",ydata);             //���y�������
    printf("���z�������:%d\n\r\n\r\n",zdata);           //���z�������
    LCD_ShowString(90,278,BLACK,GRAY,(char *)IntConvertToStr(xdata,mString));
    LCD_ShowString(140,278,BLACK,GRAY,(char *)IntConvertToStr(ydata,mString));
    LCD_ShowString(190,278,BLACK,GRAY,(char *)IntConvertToStr(zdata,mString));
   
}
//=====================================================================
//�������ƣ�userData_init
//�������أ���
//����˵����data����Ҫ��ʼ���Ľṹ������
//���ܸ�Ҫ����ʼ���û�֡�ṹ��data
//=====================================================================
void userData_init(UserData *data)            //��ʼ���û�֡�ṹ��
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
    //[2018.8.18] ���͵�����汾ȡBIOS
    uecom_version(mString);//??????????????????????
    ArrayCpy(data->softVer,mString,4);
    ArrayCpy(data->equipName,gFlashData.equipName,30);
    ArrayCpy(data->equipID,gFlashData.equipID,20);
    ArrayCpy(data->equipType,gFlashData.equipType,20);
    ArrayCpy(data->vendor,gFlashData.vendor,30);
    ArrayCpy(data->phone,gFlashData.phone,11);
    data->touchNum = 0;
    ArrayCpy(data->cmd,gFlashData.frameCmd,2);
    //����ư��2��-��ʼ������
    
}

//=====================================================================
//�������ƣ�userData_get
//�������أ���
//����˵����data����Ҫ��ֵ�Ľṹ������
//���ܸ�Ҫ�����û�֡�ṹ��data��ֵ
//=====================================================================
void userData_get(UserData *data)             //���û�֡�ṹ��data��ֵ
{
    uint16_t brightAD,mcu_temp_AD;
    char mStr[6];
    float mcu_temp;
    
    static uint32_t sn = 0;
    data->sn = sn++;    
    //��ȡmcu�¶�
    mcu_temp_AD = adc_read(AD_MCU_TEMP);       //��ȡmcu�¶�ͨ��ADֵ
    mcu_temp=TempTrans(mcu_temp_AD);           //�¶Ȼع�
    LCD_ShowString(150,105,BLUE,GRAY,(char *)FloatConvertToStr(mcu_temp,1,mStr));  //LCD����ʾоƬ�¶�
    
    data->mcuTemp =(int32_t) (mcu_temp*10);
    printf("оƬ�¶�=%6.2f\r\n",mcu_temp);
    printf("\r\n");
    
    //��ȡ����ǿ��
    brightAD = adc_read(AD_LIGHT);
    data->bright = brightAD;
    main_TimeStamp();   //��gUserData.currentTime��ֵ
    
    
    
    //����ư��1��-���ݻ�ȡ
    
    
}

//=====================================================================
//�������ƣ�LCD_Showfirst
//�������أ���
//����˵������
//���ܸ�Ҫ����ʼ��LCD�ϵ���ʾ������
//=====================================================================
void LCD_Showfirst()
{
    uint8_t temp[30] = {0};
    //uint8_t tempertature[6];
    //��1������ȫ�ֵ�ɫΪ��ɫ
    LCD_DrawSurface(0,0,240,320,GRAY);    //240*320����LCD
    //��2�����õ�һ������������
    LCD_aotu(2,2,238,38,1);               //LCDָ������͹��
    LCD_ShowString(66,15,RED,GRAY,"���«IoT-GEC"); //����
    wdog_feed();
    //��3�����õڶ�������ͨ���޹�����
    LCD_aotu(2,43,238,123,0);             //LCDָ��������
    //��ʾ�ͺ�
    LCD_ShowString(6,45,BLACK,GRAY,"[Type]                     ");
    uecom_typeGet(temp);
    temp[20]=0;
    LCD_ShowString(60,45,BLUE,GRAY,(char *)temp);
    //��ʾBIOS����汾
    wdog_feed();
    LCD_ShowString(6,65,BLACK,GRAY,"[BIOS]                      ");
    uecom_version(temp);   //ȡuecom�汾�ţ���ΪBIOS�汾�ţ�
    LCD_ShowString(58,65,BLUE,GRAY,(char *)temp);   //��ʾBIOS����汾
    //��ʾuser����汾
    LCD_ShowString(120,65,BLACK,GRAY,"[USER]        ");
    ArrayCpy(temp,gFlashData.softVer,4);
    temp[5]=0;
    LCD_ShowString(172,65,BLUE,GRAY,(char *)temp);
    //��ʾIMSI��ʾ
    LCD_ShowString(6,85,BLACK,GRAY,"[IMSI]                      ");
    //��ʾMCU�¶�
    LCD_ShowString(6,105,BLACK,GRAY,"[MCU_temperature]           ");
    
    //��4�����õ���������ͨ����أ�
    LCD_aotu(2,127,238,228,1);            //LCDָ������͹��
    LCD_ShowString(6,131,BLACK,GRAY,"[LBS]                      ");
    LCD_ShowString(6,149,BLACK,GRAY,"[Signal strength(%)]       ");
    //��ʾIP:PT (IP:PORT)
    LCD_ShowString(6,169,BLACK,GRAY,"[IP:PT]                    ");
    ArrayCpy(temp,gFlashData.serverIP,15);   
    temp[15]=0;
    LCD_ShowString(65,169,BLUE,GRAY,(char *)temp);
    LCD_ShowString(185,169,BLUE,GRAY,":");
    ArrayCpy(temp,gFlashData.serverPort,5);
    temp[5]=0;
    LCD_ShowString(195,169,BLUE,GRAY,(char *)temp);
    //��ʾ����Ƶ��
    LCD_ShowString(6,189,BLACK,GRAY,"TSI:       Freq(s):        ");
    LCD_ShowString(180,189,BLUE,GRAY,"      ");
    LCD_ShowString(180,189,BLUE,GRAY,
    (char *)IntConvertToStr(gFlashData.sendFrequencySec,temp));
    LCD_ShowString(40,189,BLUE,GRAY,"0"); //��ʾTSI������ֵ0
    LCD_ShowString(6,209,BLACK,GRAY,"Time:                      ");
    LCD_ShowString(49,209,BLUE,GRAY,"0000-00-00 00:00:00" );
    LCD_aotu(2,232,238,271,0);            //LCDָ��������
    LCD_ShowString(6,235,BLACK,GRAY,"5-year flow fee(2020-2024) ");
    
    LCD_ShowString(6,253,BLACK,GRAY,"RecvCount:                 ");
    //��5�����õ�����������״̬��ʾ����
    LCD_aotu(2,275,238,317,1);            //LCDָ������͹��
    LCD_ShowString(6,278,BLACK,GRAY,"Run State:                 ");
}
//ͼ�λ����֮main�ļ��Ӻ�����Ӵ���Graphic13��


//=====================================================================
//�������ƣ�ArrayCpy
//�������أ���
//����˵����dest�����ƺ��ŵ����飻source�������Ƶ����飻len:���Ƶĳ���
//���ܸ�Ҫ����Դ���鸴��ָ�����ȵ����ݵ�Ŀ������
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
    //������ʱ���� ʮ����
    uint16_t tyear,tmonth,tday,thour,tmin,tsec;
    uint16_t i;
    uint64_t timestamp;	    //ʱ���
    uint8_t st[20];
    //ÿ�·�֮ǰ���������������
    uint16_t MonthDay[12]={31,59,90,120,151,181,212,243,273,304,334,365};
    timestamp=0;
    //����ݻ�Ϊʮ����
    for (i=0;i<=3;i++) st[i]=gTimeString[i]-0x30;   
    tyear=st[0]*1000+st[1]*100+st[2]*10+st[3];
    //���·ݻ�Ϊʮ����
    for (i=5;i<=6;i++) st[i]=gTimeString[i]-0x30;  
    tmonth=st[5]*10+st[6];
    //���컯Ϊʮ����
    for (i=8;i<=9;i++) st[i]=gTimeString[i]-0x30;  
    tday=st[8]*10+st[9];
    //��ʱ��Ϊʮ����
    for (i=11;i<=12;i++) st[i]=gTimeString[i]-0x30;  
    thour=st[11]*10+st[12];
    //���ֻ�Ϊʮ����
    for (i=14;i<=15;i++) st[i]=gTimeString[i]-0x30;  
    tmin=st[14]*10+st[15];
    //���뻯Ϊʮ����
    for (i=17;i<=18;i++) st[i]=gTimeString[i]-0x30;  
    tsec=st[17]*10+st[18];
    //����֮ǰ��ݵ�����
    for(i=1970;i<tyear;i++)
    {
        if(i%4)
        timestamp+=31536000;
        
        else
        timestamp+=31622400;
    }
    //����֮ǰ�·ݵ�����
    if(tmonth>12) return;
    if(tmonth>2 && tyear%4)
    timestamp+=MonthDay[tmonth-2]*24*3600;
    else if(tmonth>2 && (tyear%4==0))
    timestamp+=(MonthDay[tmonth-2]+1)*24*3600;
    else if(tmonth==2)
    timestamp+=31*24*3600;
    else
    timestamp+=0;
    //���㵱ǰ�������
    if(tday>1) timestamp+=(tday-1)*24*3600;
    timestamp+=(thour*3600)+(tmin*60)+tsec;
    gUserData.currentTime=timestamp;
    
}


/*
֪ʶҪ�أ�
1.main.c��һ�������ʵ����ִ����س������̲����漰�κλ�����оƬ���⡣
���ļ����д���������漰�����Ӳ���ͻ�������ͨ��������ع�����ʵ����Ӳ��
ϵͳ�Ľ�����
2.���ļ�����������룬һ��Ϊ������ʵ����Ҫ��ɾ�������������ݾ���
��Ŀ������и��ģ���һ��Ϊ�����䡿����������������Ŀ�޹أ���ͨ�õģ�
������ݾ�����Ŀ���и��ġ�
3.���ļ��Ժ�GLOBLE_VAR�����˶��壬 �����ڰ���"includes.h"ͷ�ļ�ʱ
�ᶨ��ȫ�ֱ������������ļ��а���"includes.h"ͷ�ļ�ʱ����������ͷ�ļ���
��ȫ�ֱ�����
*/



