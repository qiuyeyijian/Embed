//======================================================================
//�ļ����ƣ�includes.h��Ӧ�ù�����ͷ�ļ���
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�20170801-20191020
//���������������õ��Ĺ������곣�����塢�Զ����������͡�����ȫ�ֱ�����
//======================================================================

#ifndef _INCLUDES_H     //��ֹ�ظ����壨��ͷ��
#define _INCLUDES_H

//----------------------------------------------------------------------
//��1��������ʵ����Ҫ��ɾ�������õ��Ĺ���
#include "user.h"


//----------------------------------------------------------------------
//��2��������ʵ����Ҫ��ɾ��ȫ��ʹ�õĺ곣���������淶��ע1��
#define TSI_VALUE 0x00CC0011 //������Ӧ��ֵ��ʵ���ã�


//----------------------------------------------------------------------
//��3��������ʵ����Ҫ��ɾ���Զ�����������
#pragma  pack(push,1)        //ȡ��4�ֽڶ���
//����������¼���«Ӳ�������������Ϣ��������flash�е���Ϣ
typedef struct FlashData
{
	//�ٲ�Ʒ��Ϣ
	uint8_t equipName[30];      //��Ʒ����
	uint8_t equipID[20];        //��Ʒ���к�
	uint8_t equipType[20];      //��Ʒ�ͺ�
	uint8_t vendor[30];         //��������  100
	uint8_t softVer[4];         //����汾
	uint64_t productTime;       //����ʱ�䣨64λʱ�����
	uint8_t userName[20];       //�û���
	uint8_t phone[11];          //�ֻ���  43
	//�ڷ�������Ϣ
	uint8_t serverIP[15];       //������IP
	uint8_t serverPort[5];      //�������˿ں�
    //���û�����flash����Ϣ
	uint32_t sendFrequencySec;  //����Ƶ��
	uint32_t resetCount;        //��λ����
	uint8_t  frameCmd[2];       //����   30
}FlashData;
//������ʵ����Ҫ��ɾ���洢�û��ɼ�������
typedef struct UserData
{
	//����������Щ����Ϊ����Ҫ�еı���
    uint8_t cmd[2];           //�û�����
	uint16_t sn;              //֡��
	uint8_t IMSI[15];         //IMSI��
	uint8_t serverIP[15];     //������IP
	uint8_t serverPort[5];    //�������˿�
	uint64_t currentTime;     //����ʱ�䣨64λʱ�����
	uint32_t resetCount;      //��λ�Ĵ���
	uint32_t sendFrequencySec;//����ʱ�������룩
	//������ʵ����Ҫ��ɾ���˴��ɰ���Ҫ��ɾ������ע�����������֡�ṹ����һ��
	uint8_t userName[20];     //�û���
	uint8_t softVer[4];       //����汾
	uint8_t equipName[30];    //��Ʒ����
	uint8_t equipID[20];      //��Ʒ���к�
	uint8_t equipType[20];    //��Ʒ����
	uint8_t vendor[30];       //��������
	int32_t mcuTemp;          //оƬ�¶�
	uint8_t surpBaseInfo[20]; //��ע1
	uint8_t phone[11];        //�ֻ���
	uint8_t IMEI[15];         //IMEI��
	uint8_t signalPower;      //�ź�ǿ��
	uint16_t bright;          //�������ȣ�����������A/Dֵ��
	uint16_t touchNum;        //��������������TSI��
	uint8_t surplusInfo[20];  //��ע2
	uint8_t lbs_location[25];
	//����ư��1��-�û��Զ����������

         //
	//����ư��2��-�û��Զ����������

}UserData;
#pragma  pack(pop)           //�ָ�4�ֽڶ���

//----------------------------------------------------------------------

//��4��������ʵ����Ҫ��ɾ������ȫ�ֱ����������淶��ע2��
//���������궨��ȫ�ֱ���ǰ׺G_VAR_PREFIX��ʵ��ԭ���ע3��
#ifdef GLOBLE_VAR                      //GLOBLE_VAR��main.c�ļ����к궨��
  #define G_VAR_PREFIX                 //ǰ׺G_VAR_PREFIX����Ϊ��
#else                                  //GLOBLE_VAR�ڷ�main.c�ļ����޶���
  #define G_VAR_PREFIX extern         //ǰ׺G_VAR_PREFIX����Ϊ"extern"
#endif
//���ڴ�����ȫ�ֱ�����
G_VAR_PREFIX uint8_t switch1;
G_VAR_PREFIX uint8_t switch2;
G_VAR_PREFIX uint8_t switch3;
G_VAR_PREFIX uint8_t switch4;
//----------------------------------------------------------------------
G_VAR_PREFIX uint8_t  gcRecvBuf[MCU_SECTORSIZE];  //���ڽ������ݻ�����
G_VAR_PREFIX uint16_t gcRecvLen;        //���ڽ��յ������ݳ��ȣ�Ϊ0ʱ��ʾû���յ�����

G_VAR_PREFIX uint16_t gcUserRecvLen;    //�û�/��������֡����
G_VAR_PREFIX uint8_t  gCount;


G_VAR_PREFIX vuint16_t gcRecvDCLen;
G_VAR_PREFIX vuint8_t gcReccrc32[4];


G_VAR_PREFIX UserData  gUserData;       //�û���Ϣ֡�ṹ��
G_VAR_PREFIX FlashData gFlashData;      //��Ҫд��flash�е�����
G_VAR_PREFIX uint64_t   gTimeSec;        //ʱ���
G_VAR_PREFIX uint8_t    gTimeString[20]; //ʱ��"2019-01-01 00:00:21/0"




//���ڴ�����ȫ�ֱ�����
G_VAR_PREFIX  uint8_t  gTime[3];        //ʱ������

//----------------------------------------------------------------------
const static FlashData flashInit[]=
{
    //012345678901234567890123456789  ��Ʒ����equipName[30]
    "[���«]NB�ն�UEģ��   ",
   //01234567890123456789  ��Ʒ���к�equipID[20]
    "2020-09-06-01     ",
   //01234567890123456789  ��Ʒ�ͺ�equipType[20]
    "AHL-STM32-NB-DX-0001",
   //012345678901234567890123456789    ��������vendor[30]
	"SD-ARM Ƕ��ʽ�˹������������� ",
   //0123  ����汾softVer[4]
    "V3.6",
	1573315200,                 //����ʱ���ʱ���2019-11-10
   //01234567890123456789  �û���userName[20]
    "NB-IoT����������Ӧ��",
   //01234567890  �ֻ���phone[11]
    "13000000000",
   //�ڷ�������Ϣ
   //012345678901234  ������IPserverIP[15]
    "116.62.63.164",           //CS-Monitorʹ��
   //01234 �������˿ں�serverPort[5]
    "20000",                   //CS-Monitor�����Ķ˿ں�
    //���û�����flash����Ϣ
    30,                          //����Ƶ��
    0,                            //��λ����
    "U0"                          //����
};

#endif                  //��ֹ�ظ����壨��β��


/*
 ֪ʶҪ�أ�
 ע1��ȫ��ʹ�õĺ곣����ʹ��ȫ��д
 ע2��ȫ�ֱ�������һ����g��ͷ�������ܱ�֤ȫ������Ψһ�ԣ�����һ��Ҫע��
      �京�壬������ʱ��׼��ֵ��
 ע3��ȫ�ֱ���һ�������ദʹ�ô������������Ϊ�˽��ȫ�ֱ����ظ�����
 �����⣬��GLOBLE_VAR����main.c������������ֻ����main.c�а���
 ��ͷ�ļ�ʱ�Ż�����ȫ�ֱ������������ļ��а������ļ�ʱ������ʱ���ͻ��Զ���
 ��ǰ׺extern��������main������������ȫ�ֱ��������ļ���ֱ��ʹ�á�
 */


