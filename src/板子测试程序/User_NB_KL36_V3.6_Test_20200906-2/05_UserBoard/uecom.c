//===================================================================
//�ļ����ƣ�uecom.c��uecom����Դ�����ļ���
//===================================================================
#include "uecom.h"    //������Ӧ����ͷ�ļ���˵����ͷ�ļ���

//�ļ�������
#define AT_reBuf_length 300
static uint8_t AT_reBuf[300];   //���UE��ATָ��ķ��ؽ��
//static uint8_t AT_reFlag = 0;   //ATָ��ɹ����صı�־,�ɹ�ʱ��ֵΪ���յ���ATָ��ص����ݳ���
static uint8_t AT_reLength = 0; //ATָ��ص����ݳ���
static uint8_t AT_haveCommand=0;//��־��ǰ�Ƿ���ATָ���
static uint8_t frameHead[2] = "V!"; //֡ͷ
static uint8_t frameTail[2] = "S$"; //֡β
//�洢��ʼ�����ȡ����IMSI��
static uint8_t IMSI[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
//�洢��ʼ�����ȡ����IMEI�� 
static uint8_t IMEI[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  

static uint8_t locationFlag=0;      //���յ�һ����Ч��λ��Ϣ��־
static uint8_t locationData[100];   //��Ž��յ���һ����Ч��λ��Ϣ


static uint8_t reip[16];    //Զ�̸���IP��ַ  ��2019/04/29�� CC ��
static uint8_t report[6];   //Զ�̸��¶˿ں�
static uint8_t reFlag = 0;  //Զ�̸��±�ʶ

//                            "1234567890123456789012345";
static uint8_t uecomType[25] = "AHL-NB-IoT-KL36       ";
static uint8_t uecomVersion[5] = "V3.6";



//======================�ڲ���������=======================================
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

//======================�ڲ�������������===================================

//========================��������========================================
//������ʹ�õ���ATָ���20180815��

#define AT        "AT\r\n"             //������ͨ��ģ��ͨѶ��ָ��
#define IPR       "AT+IPR=115200\r\n"  //��ģ�鲨��������Ϊ115200
#define ATE_0     "ATE0\r\n" //�رջ��ԣ����͸�����֮��UEģ�鲻���ԭ��������أ�
#define GSN       "AT+GSN\r\n"          //��ȡ�豸��IMEI��ָ��
#define CIMI      "AT+CIMI\r\n"         //��ȡ�豸��IMSI��ָ��

#define CFUN      "AT+CFUN=1\r\n"       //���õ绰����
#define COPS      "AT+COPS=1,2,\"46011\"\r\n"//������Ӫ��,46011��ʾ���ǵ��ŵ�
//#define COPS      "AT+COPS=1,2,\"46000\"\r\n"//������Ӫ��,46000��ʾ�����ƶ���
#define MSPCHSC   "AT*MSPCHSC=1\r\n"         //����NB֧�ֵ��㷨�����㷨
#define CPSMS     "AT+CPSMS=0\r\n"           //�ر� PSM ����
#define EGACT_0   "AT+EGACT=0,1,\"ctnb\"\r\n"//ȥ����PDP,����APN�ǵ��ŵ�
#define EGACT_1   "AT+EGACT=1,1,\"ctnb\"\r\n"//����PDP,����APN�ǵ��ŵ�
#define CSQ       "AT+CSQ\r\n"               //��ȡ�ź�ǿ��

#define ESOC      "AT+ESOC=1,1,1\r\n"        //����TCP�׽���
#define ESOSETRPT "AT+ESOSETRPT=1\r\n"   //���ý������ݵĻ�����ʽΪ��ԭʼ���ݡ�
#define ESOCL     "AT+ESOCL=0\r\n"       //�ر��׽���
#define ESOCL_1   "AT+ESOCL=1\r\n"       //�ر��׽���1
#define MENGINFO  "AT*MENGINFO=0\r\n"    //��ȡ��ǰС����Ϣ

#define ZGMODE_1 "AT+ZGMODE=1\r\n"    //����GPS��λģʽ��AGPSģʽ��
#define ZGNMEA_2 "AT+ZGNMEA=2\r\n"    //����GPS����NMEA�ϱ���ʽģʽ��RMC��ʽ��
#define ZGRUN_2 "AT+ZGRUN=2\r\n"      //����GPS����׷�ٻ�ȡ���ݣ�
#define METIME  "AT+CCLK?\r\n"       //��ȡ��վʱ��
//#define ZRST    "AT+ZRST\r\n"        //ģ�鸴λ

//AT�����Ƿ�ִ�гɹ��ı�־
#define OK        "OK"
#define ERROR     "ERROR"
//========================������������====================================

//=====================================================================
//�������أ���
//����˵����state:ͨ��ģ���Դ���������ȡֵΪ��ʹ�ú곣������
//         UECOM_OFF���ر�ͨ��ģ���Դ��
//         UECOM_ON����ͨ��ģ���Դ��
//         UECOM_REBOOT������ͨ��ģ�飨�ȹرգ���ʱ���ٿ�������
//���ܸ�Ҫ������ͨ��ģ�鹩��״̬
//�ڲ����ã�gpio_init��delay_ms
//�޸����ڣ���20180718��,LXD
//=====================================================================
void uecom_power(uint8_t state)    //����ͨ��ģ�鹩��״̬
{
    switch(state)
    {
    case UECOM_OFF:
        gpio_init(POWER_CONTROL,1,0);break;  //�ر�
    case UECOM_ON:
        gpio_init(POWER_CONTROL,1,1);break;  //��
    case UECOM_REBOOT:
        gpio_init(POWER_CONTROL,1,0);        //�ر�
        uecom_Dly_ms(2000);                  //��ʱ2��
        gpio_init(POWER_CONTROL,1,1);break;  //��
    }
}


//====================================================================
//�������أ� 0���ɹ���1����UEģ�鴮��ͨѶʧ�ܣ�2����ȡSIM����IMSI��ʧ�ܣ�
//����˵���� ��
//���ܸ�Ҫ��uecomģ���ʼ������1��������ͨѶģ��Ĵ���ͨѶ��
//                          ��2����ȡSIM����IMSI�š�
//�޸����ڣ���20180718��,LXD
//====================================================================
uint8_t uecom_init(void)
{
    //��1�����������͸���ֵ
    uint8_t ret_val ;
    //��2��ʹ�ô�����ͨ��ģ��ͨ�ţ����ȳ�ʼ������
    uart_init(UART_UE, 115200);  //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
    uart_enable_re_int(UART_UE); //��UEģ�鴮���ж�
    uecom_Dly_ms(10);
    //��3��������ͨ��ģ���ͨѶ
    if(uecom_sendCmd((uint8_t *)AT,100,2))
    {
    	uecom_power(UECOM_REBOOT);   //��UEģ�����¹���
    	uecom_Dly_ms(9000);
    }
	if(uecom_sendCmd((uint8_t *)AT,100,3))
	    goto uecom_init_err1;     //UEģ��ͨѶʧ��
    //��4���رջ��ԣ�����AT����֮��UEģ�鲻���ԭ��������أ�
    if(uecom_sendCmd((uint8_t *)ATE_0,200,3))
        goto uecom_init_err1;    //�رջ���ʧ��
    //��5����ѯģ���IMEI,����IMEI�ŷ���dest�����ǰ15���ֽ�
    if(uecom_internal_getIMEI(IMEI))
        goto uecom_init_err1;    //IMEI��ѯʧ��
    //��6����ѯģ���IMSI,����IMSI�ŷ���dest�����15-29�±�Ŀռ���
    if(uecom_internal_getIMSI(IMSI))
        goto uecom_init_err2;    //IMSI��ѯʧ��
    ret_val = 0;
    goto uecom_init_exit;
     //�����˳�
uecom_init_err1:
    ret_val = 1;            //��UEģ�鴮��ͨѶʧ��
    goto uecom_init_exit;
uecom_init_err2:
    ret_val = 2;            //��ȡSIM����IMSI��ʧ��
    goto uecom_init_exit;
    //�˳���
uecom_init_exit:
    return ret_val;
}

//=====================================================================
//�������أ� ��
//����˵���� ��
//���ܸ�Ҫ���Ͽ�Socket����
//�޸����ڣ���20190503��,
//=====================================================================
void uecom_linkOff()
{
    uart_init(UART_UE, 115200);  //��ʼ��UEģ�鴮�ڣ�������Ϊ115200
    uart_enable_re_int(UART_UE); //��UEģ�鴮���ж�
	//�ȹر�һ���׽���0����ֹͨ��ģ��δ�ϵ��Ѿ������׽��֣�
	uecom_sendCmd((uint8_t *)ESOCL,4000,1);  
	//�ȹر�һ���׽���1����ֹͨ��ģ��δ�ϵ��Ѿ������׽��֣�
	uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    
}

//=====================================================================
//�������أ� 0���ɹ������������������ӣ�1�����Ӳ���������
//����˵������
//���ܸ�Ҫ����������Ӫ�̵Ļ�վ����������������
//�޸����ڣ���20180718��,LXD
//=====================================================================
uint8_t uecom_linkBase(void)
{
	//��1����������������ֵ
	uint8_t ret=1;
	//��2�����õ绰����Ϊȫ����
	if(uecom_sendCmd((uint8_t *)CFUN,1500,2))
		goto uecom_linkBase_exit;      //���õ绰����Ϊȫ����ʧ��
	//��3������������Ӫ��
	uecom_sendCmd((uint8_t *)COPS,1500,3);
//	while(uecom_sendCmd((uint8_t *)COPS,1500,3));
	//��4������NB֧�ֵ������㷨
	uecom_sendCmd((uint8_t *)MSPCHSC,500,2);
	//��5���ر� PSM ���ܣ���Ϊ���漰���Ĳ��ԣ�
	if(uecom_sendCmd((uint8_t *)CPSMS,200,2))
		goto uecom_linkBase_exit;      //�ر� PSM ����ʧ��
	//��6������PDP
	if(uecom_sendCmd((uint8_t *)EGACT_1,10000,1))
	{
		uecom_sendCmd((uint8_t *)EGACT_0,10000,1);
		uecom_Dly_ms(8000);
		if(uecom_sendCmd((uint8_t *)EGACT_1,10000,2))
			goto uecom_linkBase_exit;  //����PDPʧ��
	}
	//���ˣ�û��ʧ���˳����ɹ���
	ret = 0;
uecom_linkBase_exit:
	    return ret;
}

//=====================================================================
//�������أ�
//����˵���� IP:�����Ӹ��·�������IP��ַ�� port:�����Ӹ��·������Ķ˿ں�
//���ܸ�Ҫ�����ø��·�������IP��ַ�Ͷ˿ں�
//�޸����ڣ���20190426��,CC
//=====================================================================
void uecom_remoteConfig(uint8_t *ip,uint8_t *port)
{
	strcpy((char *)reip,(const char *)ip);
	strcpy((char *)report,(const char *)port);
	reFlag = 1;

}

//=====================================================================
//�������أ� 0���ɹ�����TCP���ӣ�1������TCP����ʧ��
//����˵���� IP:�����ӷ�������IP��ַ�� port:�����ӷ������Ķ˿ں�
//���ܸ�Ҫ����ָ���ķ������Ͷ˿ڽ���TCP���ӡ�
//�޸����ڣ���20180718��,LXD
//=====================================================================
uint8_t uecom_linkCS(uint8_t *ip,uint8_t* port)
{
    //��1�����������͸���ֵ
    uint8_t i;
//    uint8_t flag;
    uint8_t ret_val=1;
    uint8_t ESOCON[50] = "";    //�ô�Ž���TCP���ӵ�ָ��
    uint8_t loc;
    //��2���齨����TCP���ӵ�ָ��
    strcat((char *)ESOCON,"AT+ESOCON=0,");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<5; i++)//��Ӷ˿ںţ����5λ��
    {
        if(port[i] == 0)
        {
            break;
        }
        ESOCON[loc+i] = port[i];
    }
    strcat((char *)ESOCON,",\"");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<15; i++)//��Ӷ�IP�����15λ��
    {
        if(ip[i] == 0)
        {
           break;
        }
        ESOCON[loc+i] = ip[i];
    }
    strcat((char *)ESOCON,"\"\r\n");
    //��3������TCP�׽���
    uecom_sendCmd((uint8_t *)ESOCL,4000,1);    //�ȹر�һ���׽��֣���ֹͨ��ģ��δ�ϵ��Ѿ������׽��֣�
    if(uecom_sendCmd((uint8_t *)ESOC,4000,1))
    {
    	uecom_sendCmd((uint8_t *)ESOCL,4000,1);    //�ر��׽���
    	if(uecom_sendCmd((uint8_t *)ESOC,1000,1))
    	{
    		goto uecom_linkCS_exit;
    	}
    }
    //��4������TCP����
    if(uecom_sendCmd(ESOCON,4000,2))
    {
		goto uecom_linkCS_exit;
    }
    //��5�����ý������ݵĻ�����ʽΪ��ԭʼ���ݡ�
    if(uecom_sendCmd((uint8_t *)ESOSETRPT,4000,2))
    {
		goto uecom_linkCS_exit;
    }
    ret_val = 0;
    //��2019/04/29�� CC ��
	if(reFlag == 1)
	{
		//�����û�cs�ɹ������Ӹ��·�����
		uecom_linkRemoteUpdate(reip,report);
	}
    //�˳���
uecom_linkCS_exit:
    return ret_val;
}

//=====================================================================
//�������أ� 0���ɹ�����TCP���ӣ�1������TCP����ʧ��
//����˵���� IP:�����ӷ�������IP��ַ�� port:�����ӷ������Ķ˿ں�
//���ܸ�Ҫ����ָ���ķ������Ͷ˿ڽ���TCP���ӡ���ʹ���׽���1��
//�޸����ڣ���20190504��,CC
//ע��uecom_RemoteUpdateҪ�ڵ���uecom_linkCS֮��ʹ��
//=====================================================================
uint8_t uecom_linkRemoteUpdate(uint8_t *ip,uint8_t* port)
{
    //��1�����������͸���ֵ
    uint8_t i;
//    uint8_t flag;
    uint8_t ret_val=1;
    uint8_t ESOCON[50] = "";    //�ô�Ž���TCP���ӵ�ָ��
    uint8_t loc;
    //��2���齨����TCP���ӵ�ָ��
    strcat((char *)ESOCON,"AT+ESOCON=1,");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<5; i++)//��Ӷ˿ںţ����5λ��
    {
        if(port[i] == 0)
        {
            break;
        }
        ESOCON[loc+i] = port[i];
    }
    strcat((char *)ESOCON,",\"");
    loc = strlen((const char *)ESOCON);
    for(i=0; i<15; i++)//��Ӷ�IP�����15λ��
    {
        if(ip[i] == 0)
        {
           break;
        }
        ESOCON[loc+i] = ip[i];
    }
    strcat((char *)ESOCON,"\"\r\n");
    //��3������TCP�׽���
    //�ȹر�һ���׽���0����ֹͨ��ģ��δ�ϵ��Ѿ������׽��֣�
    uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    
    if(uecom_sendCmd((uint8_t *)ESOC,4000,1))
    {
    	uecom_sendCmd((uint8_t *)ESOCL_1,4000,1);    //�ر��׽���
    	if(uecom_sendCmd((uint8_t *)ESOC,1000,1))
    	{
    		goto uecom_linkRemoteUpdate_exit;
    	}
    }
    //��4������TCP����
    if(uecom_sendCmd((uint8_t *)ESOCON,4000,2))
    {
		goto uecom_linkRemoteUpdate_exit;
    }
    //��5�����ý������ݵĻ�����ʽΪ��ԭʼ���ݡ�
    if(uecom_sendCmd((uint8_t *)ESOSETRPT,4000,2))
    {
		goto uecom_linkRemoteUpdate_exit;
    }
    ret_val = 0;

    //�˳���
    uecom_linkRemoteUpdate_exit:
    return ret_val;
}

//=====================================================================
//�������أ�  0�����ͳɹ���1����������ģʽʧ�ܣ�2�����ݷ���ʧ��
//����˵���� data:���������ݻ��������������
//		           length:���������ݵĳ���
//���ܸ�Ҫ��������ͨ���Ѿ�������TCPͨ�����ͳ�ȥ�����500���ֽڡ�
//�޸����ڣ���20180718��,LXD
//=====================================================================
uint8_t uecom_send(uint16_t length, uint8_t *data)
{
    //��1����������������ֵ
    uint8_t ret_val;
    char dataLen[5]="";        //���֡��(�ַ�����ʽ)
    char ESOSENDRAW[24]= "";   //��ŷ������������ݵ�ָ��
    uint8_t frame[500];         //�������֡
    uint16_t frameLen;          //���֡��
    frameEncode(IMSI,data, length,frame,&frameLen);//��֡
    uint_to_str(frameLen,dataLen);  //�����������ݵĳ���ת��Ϊ�ַ�����ʽ
    //��2����������ģʽ
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=0,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");
    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //��3����ʱ�ȴ���������ģʽ
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_send_exit;      //��������ģʽʧ��
	}
    //��4����ʼ��������
    uart_sendN(UART_UE ,frameLen,frame);
    //��5���ж������Ƿ��ͳɹ�
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_send_exit;         //���ݷ���ʧ��
    }
    //�������ݷ��ͳɹ�
    ret_val = 0;
    //��2019/04/29�� CC ��
	if(reFlag == 1)
	{
		//���������û�cs�ɹ������Ӹ��·�����
		uecom_linkRemoteUpdate(reip,report);
		//����³�����������
		uecom_updateSend(9,(uint8_t *)"heartbeat");
	}
    uecom_send_exit:
    return ret_val;
}

//=====================================================================
//�������أ�  0�����ͳɹ���1����������ģʽʧ�ܣ�2�����ݷ���ʧ��
//����˵���� data:���������ݻ��������������
//		           length:���������ݵĳ���
//���ܸ�Ҫ��������ͨ���Ѿ�������TCPͨ�����ͳ�ȥ��
//         ���500���ֽڡ���ʹ���׽���0���ͣ�
//�޸����ڣ���20190504��,CC
//=====================================================================
uint8_t uecom_updateSend(uint16_t length, uint8_t *data)
{
    //��1����������������ֵ
    uint8_t ret_val;
    char dataLen[5]="";        //���֡��(�ַ�����ʽ)
    char ESOSENDRAW[24]= "";   //��ŷ������������ݵ�ָ��
    uint8_t frame[100];         //�������֡
    uint16_t frameLen;          //���֡��
//    uint8_t i,ch,flag,count;
//    uint8_t sendBack[50];
    frameEncode(IMSI,data, length,frame,&frameLen);//��֡
    uint_to_str(frameLen,dataLen);   //�����������ݵĳ���ת��Ϊ�ַ�����ʽ
    //��2����������ģʽ
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=1,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");
    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //��3����ʱ�ȴ���������ģʽ
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_updateSend_exit;      //��������ģʽʧ��
	}
    //��4����ʼ��������
    uart_sendN(UART_UE ,frameLen,frame);
    //��5���ж������Ƿ��ͳɹ�
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_updateSend_exit;         //���ݷ���ʧ��
    }
    //�������ݷ��ͳɹ�
    ret_val = 0;
    uecom_updateSend_exit:
    return ret_val;
}


//=====================================================================
//�������ƣ�uecom_transparentSend
//�������أ�  0�����ͳɹ���1����������ģʽʧ�ܣ�2�����ݷ���ʧ��
//����˵���� data:���������ݻ��������������
//		           length:���������ݵĳ���
//���ܸ�Ҫ��͸�����ͣ�������ͨ���Ѿ�������TCPͨ�����ͳ�ȥ�����500���ֽڡ�
//�޸����ڣ���20180922��,LXD
//=====================================================================
uint8_t uecom_transparentSend(uint16_t length, uint8_t *data)
{
    //��1����������������ֵ
    uint8_t ret_val;
    char dataLen[5]="";        //���֡��(�ַ�����ʽ)
    char ESOSENDRAW[24]= "";   //��ŷ������������ݵ�ָ��
//    uint8_t frame[500];         //�������֡
//    uint16_t frameLen;          //���֡��
//    frameEncode(IMSI,data, length,frame,&frameLen);//��֡
    uint_to_str(length,dataLen);
    //��2����������ģʽ
    strcat(ESOSENDRAW,"AT+ESOSENDRAW=0,");
    strcat(ESOSENDRAW, dataLen);
    strcat(ESOSENDRAW, "\r\n");

    uart_send_string(UART_UE,(uint8_t *)ESOSENDRAW);
    //��3����ʱ�ȴ���������ģʽ
	if(uecom_sendCmd((uint8_t *)"wait",2000,1))
	{
		ret_val = 1;
		goto uecom_send_exit;      //��������ģʽʧ��
	}
    //��4����ʼ��������
    uart_sendN(UART_UE ,length,data);
    //��5���ж������Ƿ��ͳɹ�
    if(uecom_sendCmd((uint8_t *)"SEND",2000,1))
    {
    	ret_val = 2;
    	goto uecom_send_exit;         //���ݷ���ʧ��
    }
    //�������ݷ��ͳɹ�
    ret_val = 0;
    uecom_send_exit:
    return ret_val;
}


//====================================================================
//�������أ���
//����˵����ch�������жϽ��յ������ݣ�length�����յ����������ݳ��ȣ�
//                   recvData���洢���յ�����������
//���ܸ�Ҫ����������Ҫ���ڴ����ж��С�����Ҫ���봮���жϽ��յ������ݡ�
//  �����������й���ʵ�־�������api����apiʵ�ֵĹ��ܣ�
//  ��1���������緢����������;��2�������ڲ�ʹ�ñ�api��ģ��������ݽ���
//�޸����ڣ���20180718��,LXD
//=====================================================================
void uecom_interrupt(uint8_t ch,uint16_t *length,uint8_t recvData[])
{
	//�������ݵ���������
	if(AT_haveCommand)
	{
		AT_reBuf[AT_reLength] = ch;    //�洢���յ������ݵ�������AT_reBuf��
		//���յ��������±����
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;
		AT_reBuf[AT_reLength] = 0;
	}
	uecom_recv(ch,length,recvData); //�������Է�����������
	if(locationFlag==0)
	{
		uecom_recvLocation(ch,locationData);//����λ��Ϣ
	}
}


//====================================================================
//�������أ���
//����˵����ch�������жϽ��յ������ݣ�length�����յ����������ݳ��ȣ�
//                   recvData���洢���յ�����������
//���ܸ�Ҫ��͸�����жϴ���������������Ҫ���ڴ����ж��С�
//         ����Ҫ���봮���жϽ��յ������ݡ�
//  �����������й���ʵ�־�������api����apiʵ�ֵĹ��ܣ�
//  ��1���������緢����������;��2�������ڲ�ʹ�ñ�api��ģ��������ݽ���
//�޸����ڣ���20180922��,LXD
//=====================================================================
void uecom_transparentInterrupt(uint8_t ch,uint16_t *length,
		uint8_t recvData[])
{
	//�������ݵ���������
	if(AT_haveCommand)
	{
		AT_reBuf[AT_reLength] = ch;    //�洢���յ������ݵ�������AT_reBuf��
		AT_reLength = (AT_reLength+1)%AT_reBuf_length;//���յ��������±����
		AT_reBuf[AT_reLength] = 0;
	}
	uecom_transparentRecv(ch,length,recvData); //�������Է�����������
	if(locationFlag==0)
	{
		uecom_recvLocation(ch,locationData);//����λ��Ϣ
	}
}
//=====================================================================
//�������أ�0������GNSS�ɹ���1������GNSSʧ��
//����˵����state������GNSS�Ŀ���״̬��1����������2����������3����������
//          ����Ĭ��ʹ����������
//���ܸ�Ҫ������GNSS��״̬��������ر�GNSS�����趨������ʽ��
//�޸����ڣ���20180902����
//=====================================================================
uint8_t uecom_gnssSwitch (uint8_t state)
{
	//��1������ʹ�õ��ı���
	uint8_t  ret;
	ret = 1;    //Ĭ�Ϸ���ʧ��
	//��2�����ݴ���ֵ����GNSS
	switch(state)
	{
	case 1:
        if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//����GPS��λģʽ��AGPSģʽ��
        	goto uecom_gnssSwitch_exit;
    	if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//����GPS����NMEA�ϱ���ʽģʽ��RMC��ʽ��
    	{
    		goto uecom_gnssSwitch_exit;
    	}

		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//����ѭ����ȡ��λ��Ϣ
			goto uecom_gnssSwitch_exit;
        break;
	case 2:
		if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//����GPS��λģʽ��AGPSģʽ��
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//����GPS����NMEA�ϱ���ʽģʽ��RMC��ʽ��
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//����ѭ����ȡ��λ��Ϣ
			goto uecom_gnssSwitch_exit;
		break;
	case 3:
		if(uecom_sendCmd((uint8_t *)ZGMODE_1,1000,2))//����GPS��λģʽ��AGPSģʽ��
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGNMEA_2,1000,2))//����GPS����NMEA�ϱ���ʽģʽ��RMC��ʽ��
			goto uecom_gnssSwitch_exit;
		if(uecom_sendCmd((uint8_t *)ZGRUN_2,1000,2))//����ѭ����ȡ��λ��Ϣ
			goto uecom_gnssSwitch_exit;
		break;
	default:
		break;
	}
	//���ˣ��������������
	ret = 0;
	uecom_gnssSwitch_exit:
	return ret;
}


//====================================================================
//�������أ�0����ȡ��λ��Ϣ�ɹ���1��û�л�ö�λ��Ϣ
//����˵����data���洢��õ�GNSS�����Ϣ�����ýṹ��ķ�ʽ��������4����Ա��
//       time��15���ֽڵ�uint8_t���飩,����:��20180706155132����ʾ2018��7��6��15:51:32
//       latitude��double���ͣ���γ����Ϣ��longitude��double����������Ϣ��
//       speed��double���ͣ����ٶȣ���λΪ����ÿ�롣
//       attitude��double���ͣ������θ߶ȣ���λΪ����
//���ܸ�Ҫ�������GNSS��λ��ص���Ϣ��
//�޸����ڣ���20180902��
//=====================================================================
uint8_t uecom_gnssGetInfo (UecomGnssInfo *data)   // UecomGnssInfoΪ�洢gnss��Ϣ�Ľṹ������
{
	//��1�����屾����ʹ�õı���
	uint8_t i,j,ret;
	uint8_t *p;
	double a,b,c,d;
	//��2���ж��Ƿ��ȡ����Ч�Ķ�λ����
	ret = 1;        //����Ĭ�Ϸ���1
	if(locationFlag==0)
		goto uecom_gnssGetInfo_exit;
	//��3����ͨ��ģ�鷵�������н�����UTCʱ��
	//��3.1����ȡ����
	p =(uint8_t *) strstr((const char *)locationData,(const char *)",");   //����ַ�','��һ�����ڵ�λ��
	j=0;                        //������Ƕ���
	while(j<9)                  //��λ����9������
	{
		if(*p==',')
			j++;
		p++;
	}
	data->time[0]='2';//�������
	data->time[1]='0';
	data->time[6]=*p;          //��ȡ����
	data->time[7]=*(++p);
	data->time[4]=*(++p);
	data->time[5]=*(++p);
	data->time[2]=*(++p);
	data->time[3]=*(++p);
	//��4.2����ȡʱ��
	p =(uint8_t*) strstr((const char *)locationData,(const char *)",");   //����ַ�','��һ�����ڵ�λ��
	i=8;                        //������Ƕ����������±�
	while(i<14)                 //��ȡʱ��
	{
		p++;
		data->time[i]=*p;
		i++;
	}
	data->time[i]=0;                   //��ӿղ���������ַ���
	//���ˣ���ȡUTCʱ��ɹ�
	//��4����ȡλ����Ϣ
	//��4.1����ȡ��ά����Ϣ
	p = (uint8_t *)strstr((const char *)locationData,(const char *)",");   //����ַ�','��һ�����ڵ�λ��
	j=0;                        //������Ƕ���
	while(j<3)                  //��λ����3������
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
	++p; //����'.'
	c = 0.1;
	b = b + ((*p) - '0')*c;
	while((*(++p))!=',')
	{
		c *= 0.1;
		b = b + ((*p) - '0')*c;
	}
	++p;    //����','
	if((*p) == 'N')
		d = 1;
	else
		d = -1;
	data->latitude = (a + b/60.0)*d;
	//��4.2����ȡ��������Ϣ
	++p;
	++p;   //�������ţ�����������Ϣλ��

	a=((*p)-'0')*100.0;
	a=a+((*(++p))-'0')*10.0;
	a=a+((*(++p))-'0');

	b = ((*(++p))-'0')*10.0 ;
	b=+ ((*(++p))-'0');


	++p;
	++p;   //����'.'
	c = 0.1;
	b = b + ((*p) - '0')*c;
	while((*(++p))!=',')
	{
		c *= 0.1;
		b = b + ((*p) - '0')*c;
	}
	++p;    //����','
	if((*p) == 'E')
		d = 1;
	else
		d = -1;
	data->longitude = (a + b/60.0)*d;
	//��5����ͨ��ģ�鷵�������н������ٶ���Ϣ�������봫��Ľṹ��
	if(strstr((const char *)locationData,(const char *)"E,") != NULL)
		 p =(uint8_t *) strstr((const char *)locationData,(const char *)"E,");
	else
		 p =(uint8_t *) strstr((const char *)locationData,(const char *)"W,");
	p++;
	p++;//�����Ե��ٶȴ�
	a = 0;
	while((*p)!='.')
	{
		a = a*10 + (*p - '0');
		p++;
	}
	p++;//������
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
	//��6����ͨ��ģ�鷵�������н�����������Ϣ�������봫��Ľṹ��
	data->attitude = 0;//�޺�����Ϣ
	//���ˣ����ݽ������
	locationFlag=0;    //��ս��յ�һ����Ч��λ��Ϣ��־
	ret = 0;
	uecom_gnssGetInfo_exit:
	return ret;
}

//====================================================================
//�������أ�0����ȡ��վ��Ϣ�ɹ���1����ȡ�ź�ǿ��ʧ�ܣ�
//����˵����retData���洢���ص���Ϣ,���ٷ���20���ֽڡ�
//          ��Ϣ��ɣ��ź�ǿ��(1���ֽ�)+��վ�ţ�19���ֽڣ�
//���ܸ�Ҫ����ȡ���վ��ص���Ϣ���ź�ǿ�Ⱥͻ�վ��
//�޸����ڣ���20180815��,WB
//=====================================================================
uint8_t uecom_baseInfo (uint8_t retData [20])
{
	//��1�����������͸���ֵ
	uint8_t i,j,*p,ret;
	//��2����ȡ�ź�ǿ�ȣ�����ȡ���Σ����1��
	ret = 1;           //���ô��󷵻�Ϊ1
	for(j=0;j<3;j++)
	{
		//��2.1����ģ�鷢�ͻ�ȡ�ź�ǿ�ȵ�ָ��
		if( uecom_sendCmd((uint8_t *)CSQ,500,3))
			goto uecom_baseInfo_exit;
		//��2.2���ӽ��յ��������н������ź�ǿ�ȣ�תΪuint8_t��ʽ��������*signalPower
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
	if(j==3)goto uecom_baseInfo_exit;  //��3�ξ�ʧ�ܣ�����Ϊ��ȡ�ź�ǿ��ʧ��
    //��3����û�վ��
	ret = 2;           //���ô��󷵻�Ϊ2
	//��3.1����ģ�鷢�ͻ�ȡ��վ�ŵ�ָ��
	if(uecom_sendCmd((uint8_t *)MENGINFO,500,3))
		goto uecom_baseInfo_exit;
	//��3.2���ӽ��յ��������н�������վ�ţ�������retData������
	p =(uint8_t *) strstr((const char *)AT_reBuf,(const char *)"*MENGINFOSC:");   //����ַ�':'���ڵ�λ�ã����Ϊ��������ݡ�
	j = 1;
	i = 0;
	strncpy((char *)retData+1,(const char *)"460,11,",7);
	j+=7;
	while(i<3)//����������������
	{
		if(*p == ',')
			i++;
		p++;
	}
	++p;//����cell ID��
	while((*p)!='\"')//��ȡcell ID
	{
		retData[j++] = *p;
		p++;
	}
	retData[j++] =',';
	while(i<9)//�������ھŸ�����
	{
		if(*p == ',')
			i++;
		p++;
	}
	++p;//����TAC��
	while((*p)!='\"')//��ȡTAC
	{
		retData[j++] = *p;
		p++;
	}
	if(j<=0)  //δ�ܳɹ�����������
		goto uecom_baseInfo_exit;   //�˳�����
	//���һ������ɾ����������Ϊ�ַ���������
	retData[j]=0;
    //��4���������ˣ�˵��ִ�гɹ�
    ret = 0;          //����ֵ��Ϊ0
    uecom_baseInfo_exit:
    return ret;
}

//====================================================================
//�������أ�0����ȡģ����Ϣ�ɹ���1����ȡģ����Ϣʧ��
//����˵����retData���洢���ص���Ϣ,���ٷ���40���ֽڡ�
//          ��Ϣ��ɣ�IMEI (20���ֽ�)+IMSI��20���ֽڣ�
//���ܸ�Ҫ�������Ҫ��ģ����ص���Ϣ��������IMEI�ţ�IMSI��
//�޸����ڣ���20180718��,LXD
//=====================================================================
uint8_t uecom_modelInfo (uint8_t retData[40])
{
	int i;
	//��1���ӱ�������ȫ�ֱ�����ȡ��IMEI����ֵ
	for(i=0;i<15;i++)
		retData[i]=IMEI[i];
	retData[i] = 0;    //������
	//��2���ӱ�������ȫ�ֱ�����ȡ��IMSI����ֵ
	for(i=20;i<35;i++)
		retData[i]=IMSI[i-20];
	retData[i] = 0;   //������
	return 0;
}

//======================================================================
//�������أ���
//����˵����type:���«�ͺ�
//���ܸ�Ҫ����ȡ���«�ͺ�
//======================================================================
uint8_t uecom_typeGet(uint8_t *type)
{
	memcpy(type,uecomType,sizeof(uecomType));
	return 0;
}

//======================================================================
//�������أ���
//����˵����biosVer:���«�汾��ͨ��ģ���ͺţ�
//���ܸ�Ҫ����ȡ���«�汾
//======================================================================
void uecom_version(uint8_t *version)
{
	memcpy(version,uecomVersion,sizeof(uecomVersion));
}

//====================================================================
//�������أ�0�����get����ɹ���1����ʼ��httpʧ�ܣ�2������url����ʧ�ܣ�
//			3����������ʧ�ܣ�4����������ʧ�ܣ�5����������ʧ�ܣ�
//          6����������ʧ�ܣ�7����÷���ʧ�ܣ�
//����˵����ip:Ŀ���������ַ��port :Ŀ���ַ��
//		 	url:get��������ݡ�result:get���󷵻صĽ����
//          ���鳤����Ԥ�Ʒ��صĳ��ȣ��û�Ӧ��֪�������ݣ�*1.5��������
//���ܸ�Ҫ������http��get���󣬲������ؽ���洢��result��
//=====================================================================
uint8_t uecom_httpGet (uint8_t ip[],uint8_t port[],uint8_t url[],
		uint8_t result[])
{
	return 1;
}

//------����Ϊ���������õ��ڲ�����--------------------------------------------
//====================================================================
//�������أ�0��ָ��ͳɹ�;1��ָ���ʧ��
//����˵����cmd����Ҫ���͵�ATָ����׵�ַ����"wait"����"SEND";
//        ���ֽ�;maxDelayMs:ÿ���������ȴ�ʱ��;maxTimes:��෢�����������
//ע��"wait"�ж��Ƿ���Է������ݣ������֡�CONNECT������
//    "SEND"�����жϷ��������Ƿ�ɹ���
//���ܸ�Ҫ������ATָ���ȡ������Ϣ��
//�޸����ڣ���2018-06-30��,LXD
//====================================================================
uint8_t uecom_sendCmd(uint8_t *cmd,uint16_t maxDelayMs,uint16_t maxTimes)
{
	//�жϷ�ʽʵ��
    //��1�����������͸���ֵ
	uint8_t ret;
	uint16_t i,j,count;
	//��ŷ���ATָ��������е��ַ�����Ĭ��Ϊ"OK"
	uint8_t waitStr[30]="OK";      
	AT_reLength = 0;
	//����ַ��������ڽ����ж����ٽ���һ���ֽ�ʱ��������λ�ø�ֵ0��
	AT_reBuf[0]=0;
	//�����жϷ����꿪����������ָ�AT+ESOSENDRAW����
	if(strcmp(( const char *)cmd,( const char *)"wait")==0)
		//�Ƿ��յ�CONNECT���Ӷ�ȥ��������
		strcpy((char *)waitStr,( const char *)"CONNECT"); 
		
		//cmd="wait";waitstr="CONNECT"
		//cmd;waitstr="OK"
		
	AT_haveCommand=1;   //��λAT�����־
	uecom_Dly_ms(10);  //��ֹ����AT����͹���Ƶ��
	j = maxDelayMs/100;
	uart_enable_re_int(UART_UE);//�����ж�
	for(count=0;count<maxTimes;count++)
	{
		//�����ATָ��
		if(strcmp(( const char *)cmd,( const char *)"SEND") 
			&& strcmp(( const char *)cmd,( const char *)"wait"))
		{
			AT_reLength = 0;
			//����ַ��������ڽ����ж����ٽ���һ���ֽ�ʱ��������λ�ø�ֵ0��
			AT_reBuf[0]=0;
			uart_send_string(UART_UE,cmd); //ͨ�����ڷ���ATָ��
		}
		//�ȴ�maxDelayMs����
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
	        	uecom_Dly_ms(200);//����100����
	        	break;
	        }
		}
		//���ˣ�˵��δ���յ���ȷ���ء�
    	j=j*2; //��ʧ�ܣ���һ�η���ָ��ʱ���ȴ�ʱ��ӱ�
    	if (j>(maxDelayMs/100)*3) j=(maxDelayMs/100)*3;
    	
	}
	ret = 1;
	uecom_sendCmd_exit:
	AT_haveCommand=0;
	return ret;
}

//==================================================================
//�������أ� 0����ȡIMEI�ɹ���1����ȡIMEI��ʧ�ܣ�2������IMEI��ʧ��
//����˵����dest����ŷ��ص�IMEI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMEI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ���2018-06-30��,LXD
//==================================================================
uint8_t uecom_internal_getIMEI(uint8_t *dest)
{
    //��1�����������͸���ֵ
    uint8_t ret_val;
    uint8_t *p,*start,*end;
    //��2����ȡIMEI��
    dest[15] = 0;        //�ַ����Ľ�β��
    if(uecom_sendCmd((uint8_t *)GSN,200,4))
    {
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)GSN,200,5))
		{
			ret_val = 1;
			goto uecom_internal_getIMEI_exit;      //��ȡIMEI��ʧ��
		}
    }

    //strstr(str1,str2) ���������ж��ַ���str2�Ƿ���str1���Ӵ���
    //����ǣ���ú�������str2��str1���״γ��ֵĵ�ַ�����򣬷���NULL��

    //��3����IMEI�Ŵ洢��dest������
    p =(uint8_t *) strstr((const char *)AT_reBuf,(const char *)"OK");
    while(p>=AT_reBuf&&(*p<'0'||*p>'9'))p--;    //���ҵ����һ���������ڵ�λ��
    end=p;
    while(p>=AT_reBuf && *p>='0' && *p<='9')p--;//���ҵ���һ���������ڵ�λ��
    start=p+1;
    if(start>end)
    {
        ret_val = 2;
        goto uecom_internal_getIMEI_exit;      //���ص������в���IMEI��
    }

    //void *memcpy(void *dest, const void *src, size_t n);
    //��Դsrc��ָ���ڴ��ַ����ʼλ�ÿ�ʼ����n���ֽڵ�Ŀ��dest��ָ���ڴ��ַ����ʼλ����

    memcpy(dest,start,end-start+1);//������ȡ��������
    dest[end-start+1]=0;//������
    ret_val = 0;
    //�˳���
uecom_internal_getIMEI_exit:
    return ret_val;
}

//==================================================================
//�������أ�0����ȡIMSI�ɹ���1����ȡIMSIʧ��
//����˵����dest����ŷ��ص�IMSI�ţ�15λ
//���ܸ�Ҫ����ȡ�豸IMSI��
//�ڲ����ã�uecom_sendCmd
//�޸����ڣ���2018-06-30��,LXD
//==================================================================
uint8_t uecom_internal_getIMSI(uint8_t *dest)
{
    //��1�����������͸���ֵ
    uint8_t ret_val;
//    uint8_t i,k;
    uint8_t *p,*start,*end;
    dest[14] = 0;
    //��2����ȡIMSI��
	if(uecom_sendCmd((uint8_t *)CIMI,6000,2))
	{
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)CIMI,6000,1))
		{
			ret_val = 1;
			goto uecom_internal_getIMSI_exit;     //��ȡIMSI��ʧ��
		}
	}

	//strstr(str1,str2) ���������ж��ַ���str2�Ƿ���str1���Ӵ���
	//����ǣ���ú�������str2��str1���״γ��ֵĵ�ַ�����򣬷���NULL��

    //��3����IMSI�Ŵ洢��dest������
    p = (uint8_t *)strstr((const char *)AT_reBuf,(const char *)"OK");
    while(p>=AT_reBuf&&(*p<'0'||*p>'9'))p--;    //���ҵ����һ���������ڵ�λ��
    end=p;
    while(p>=AT_reBuf && *p>='0' && *p<='9')p--;//���ҵ���һ���������ڵ�λ��
    start=p+1;
    if(start>end)
    {
        ret_val = 2;
        goto uecom_internal_getIMSI_exit;       //���ص������в���IMSI��
    }

    //void *memcpy(void *dest, const void *src, size_t n);
    //��Դsrc��ָ���ڴ��ַ����ʼλ�ÿ�ʼ����n���ֽڵ�Ŀ��dest��ָ���ڴ��ַ����ʼλ����

    memcpy(dest,start,end-start+1);             //������ȡ��������
    dest[end-start+1]=0;                        //������
    ret_val = 0;
    //�˳���
uecom_internal_getIMSI_exit:
    return ret_val;
}

//====================================================================
//�������أ���
//����˵����ch:��ת����������        *str�����ת������ַ���
//���ܸ�Ҫ�����޷�������ת��Ϊ�ַ���
//====================================================================
void uint_to_str(uint32_t ch,char *str)
{
    int i,j,sign;
    char s[10];
    if((sign=ch)<0)//��¼����
        ch=-ch;//ʹn��Ϊ����
    i=0;
    do{
           s[i++]=ch%10+'0';//ȡ��һ������
    }
    while ((ch/=10)>0);//ɾ��������
    if(sign<0)
      s[i++]='-';
    for(j=i-1;j>=0;j--)//���ɵ�����������ģ�����Ҫ�������
           str[i-1-j]=s[j];
    str[i]=0;
}

//====================================================================
//���ܸ�Ҫ��ʵ�ִ��������ݵ���֡,�����������ݼ���֡ͷ��֡����֡β�Լ�У����Ϣ
//       frame=֡ͷ+IMSI+data����+data+CRCУ����+֡β
//����˵����imsi���������ݵ��豸��IMSI��
//       data:����֡������ͷָ��
//       dlen:����֡�����ݳ���
//       frame:��֮֡��Ĵ���������֡
//       framelen:��֡������ݳ���
//�������أ���
//====================================================================
void frameEncode(uint8_t *imsi,uint8_t *data,uint16_t dlen,
                 uint8_t *frame,uint16_t *framelen)
{
    uint16_t crc,len;

    //��֡����
    //֡ͷ
    frame[0] = frameHead[0];
    frame[1] = frameHead[1];
    len = 2;
    //IMSI��
    strncpy((char *)frame+2,(const char *)imsi,15);
    len += 15;
    //data����
    frame[len++] = dlen>>8;
    frame[len++] = dlen;
    //data
    memcpy(frame+19,data,dlen);
    len += dlen;
    //CRCУ����
    
    //��IMSI�š�data���Ⱥ�data����CRCУ��
    crc = ue_crc16((uint8_t *)(frame+2),dlen+17);
    frame[len++] = crc>>8;
    frame[len++] = crc;
    //֡β
    frame[len++] = frameTail[0];
    frame[len++] = frameTail[1];
    frame[len] = 0;
    *framelen = len;
}

//====================================================================
//�������أ�0�����յ���ͨ��TCP/UDP���������ݣ�1�����ڽ�֡��2�������ݴ���
//����˵����ch�����ڽ��յ�������(1�ֽ�)��*dataLen:�洢���յ������ݳ��ȣ�
//      *data:�����յ������ݴ�ŵ���������;
//���ܸ�Ҫ���Խ��յ����Է����������ݽ��н�֡��������Ч���ݴ���data����֮�У�
//          �ɴ��ڵ�uart�жϵ��á�
//�޸����ڣ���2018-06-30��,LXD
//====================================================================
uint8_t uecom_recv(uint8_t ch,uint16_t *dataLen, uint8_t *data)
{
    static uint16_t index1 = 0;   //֡����
    static uint16_t length = 0;  //���ݳ���
    uint8_t ret_val;
    uint16_t i;
    uint8_t MCRC[2];
    uint16_t mcrc;
    //���δ����֡ͷ����δ�յ�֡ͷ���������(����������֡)
    if((index1 == 0 && ch != frameHead[0]) ||
      (index1 == 1 && ch != frameHead[1]))
    {
    	index1 = 0;
        length = 0;
        ret_val = 2;     //�������ݴ���
        goto uecom_recv_exit;
    }
    //����˵�������Է�����������
    data[index1++] = ch;  //�洢֡����
    if(index1 == 19)      //��ȡdata����
    {
        length = ((uint16_t)data[17]<<8) + data[18];
    }
    //���յ������ݴﵽһ֡���ȡ�length+23Ϊ����֡����
    if(length != 0 && index1 >= length+23)
    {
        //CRCУ��
        mcrc = ue_crc16((uint8_t *)data+2,length+17);
        MCRC[0] = (mcrc>>8)&0xff;
        MCRC[1] = mcrc&0xff;
        if(data[index1-2]!=frameTail[0] ||
        		data[index1-1]!= frameTail[1] //δ����֡β
            || MCRC[0] != data[length+19] ||
			MCRC[1] != data[length+20])       //CRC�������
        {
        	index1 = 0;
            length = 0;
            ret_val = 2;         //�������ݴ���
            goto uecom_recv_exit;
        }
        for(i=0;i<length;i++)
        {
            data[i] = data[i+19];//19Ϊ��Ч�ֽ�ǰ�����ݳ���
        }
        *dataLen = length;

        index1 = 0;
        length = 0;
        ret_val = 0;             //���յ���ͨ��TCP/UDP����������
        goto uecom_recv_exit;
    }
    *dataLen = 0;
    ret_val = 1;//���ڽ�֡
uecom_recv_exit:
    return ret_val;
}

//=====================================================================
//�������أ�0�����յ���ͨ��TCP/UDP���������ݣ�1��δ���յ�����
//����˵����ch:���ڽ��յ�������
//                   *dataLen:�洢���յ������ݳ���
//                   *data:�����յ������ݴ�ŵ������鴮��;
//���ܸ�Ҫ��͸�����պ����������յ������ݴ���data����֮�У��ɴ��ڵ�uart�жϵ��á�
//�޸����ڣ���20180922��,LXD
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
	//��֡��ʽ
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
//�������ƣ�uecom_recvLocation
//�������أ�0�����յ��˶�λ��Ϣ��1���������ݴ���
//����˵����ch�����ڽ��յ�������(1�ֽ�); *data:�����յ������ݴ�ŵ���������;
//���ܸ�Ҫ�������յ���һ�ж�λ��Ϣ����data����֮�У��ɴ��ڵ�uart�жϵ��á�
//�޸����ڣ���2018-07-17��,LXD
//====================================================================
uint8_t uecom_recvLocation(uint8_t ch, uint8_t *data)
{
	static uint16_t index2 = 0;   //��������
    uint8_t ret_val,*p,j;
    ret_val=0;
    //������յ�ǰ�����ֽڲ���$GN,����Ϊ���Ƕ�λ��Ϣ
    if((index2 == 0 && ch != '$')
    	||(index2 == 1 && ch != 'G')
		||(index2 == 2 && ch != 'N')
		||(index2 == 3 && ch != 'R')
		||(index2 == 4 && ch != 'M')
		||(index2 == 5 && ch != 'C')
		)
    {
    	index2 = 0;
        ret_val = 1;     //�������ݴ���
        goto uecom_recv_exit;
    }
    //����˵���Ƕ�λ��Ϣ
    data[index2++] = ch;      //�洢���յ���λ��Ϣ
    if(ch=='\n') //������յ�һ����Ч��λ��Ϣ
    {
    	if(data[17]=='A')
    	{
    		p =(uint8_t *) strstr((const char *)data,(const char *)",");   //����ַ�','��һ�����ڵ�λ��
    		j=0;                    //������Ƕ���
    		while(j<9)              //��λ����9������
    		{
    			if(*p==',')
    				j++;
    			p++;
    		}
    		if((*p)!=',')        //������յ����ڣ��ھŸ����ź������ڣ�
    		{
    			locationFlag=1;//��λ���յ�һ����Ч��λ��Ϣ��־
    			index2 = 0;
    			ret_val = 0;     //���յ�һ�ж�λ��Ϣ
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
//�������أ��ɹ�������ʱ�����1970.1.1������������������   ʧ�ܣ�����0
//����˵������
//���ܸ�Ҫ����ȡ������վ��ʱ�䣬��ʱ����ĸ�ʽ���أ�Ҫ����ͨ��ģ�����
//          ��վ��ʼ�������ʹ��
//======================================================================
void uecom_getTime(uint8_t dateform[20])
{
	//��1�����������͸���ֵ
	uint8_t m;
	uint8_t statu;
	uint16_t i,j;
	//������ʱ���� ʮ����
	int tyear=0;
	uint16_t tmonth;
	uint16_t tday;
	uint16_t thour;
	uint16_t tmin;
	uint16_t tsec;
	uint8_t datetime[20];
	//��2�����ͻ�ȡʱ��ָ��
	for(m=0;m<50;m++) AT_reBuf[m]=0;
	if(uecom_sendCmd((uint8_t *)METIME,200,1))
	{
		uecom_sendCmd((uint8_t *)AT,100,4);
		if(uecom_sendCmd((uint8_t *)METIME,200,1))
		{
			//ret_val = 0;
			goto uecom_getTime_exit;     //��ȡIMSI��ʧ��
		}
	}
	uecom_Dly_ms(50);
	i=0;
	statu=0;
	//���˶����ַ�
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
	if(statu!=3) return;  //�ַ������󣬷��س�ʼʱ��
	ArrayCopy(AT_reBuf,AT_reBuf+i,50-i);     //�ַ���ǰ��
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
	//���϶�������ʱ��
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
        	dateform[t]=datetime[t]+0x30;  //תΪASCII�ַ�
        }
    }
	AT_reLength = 0;
	AT_reBuf[0]=0;//����ַ��������ڽ����ж����ٽ���һ���ֽ�ʱ��������λ�ø�ֵ0��
	uecom_Dly_ms(100);
	//�˳���
   uecom_getTime_exit:
   return;
}

//=====================================================================
//���ܸ�Ҫ�������ݽ���16λ��CRCУ�飬����У���Ľ��ֵ
//����˵����ptr:��ҪУ������ݻ�����
//      len:��Ҫ��������ݳ���
//�������أ�����õ���У��ֵ
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
//�������أ���
//����˵����dest�����ƺ��ŵ����飻source�������Ƶ����飻len:���Ƶĳ���
//���ܸ�Ҫ����Դ���鸴��ָ�����ȵ����ݵ�Ŀ������
//=====================================================================
void ArrayCopy(uint8_t * dest,uint8_t * source,uint16_t len)
{
	uint16_t i = 0;
	for(i=0;i<len;i++)
		dest[i]=source[i];
}

//======================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ����ʱ - ���뼶
//======================================================================
void uecom_Dly_ms(uint16_t ms)
{
	for (uint32_t ys = 0; ys < (7000*ms); ys++)  __asm ("nop");
}