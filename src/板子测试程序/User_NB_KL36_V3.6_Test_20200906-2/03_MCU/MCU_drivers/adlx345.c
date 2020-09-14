//======================================================================
//�ļ����ƣ�adlx345.c
//���ܸ�Ҫ��С�ƹ���ͷ�ļ�
//������λ���մ�arm��������(sumcu.suda.edu.cn)
//���¼�¼��20190309 ZYL
//======================================================================

#include "adlx345.h"    //����adlx345ͷ�ļ�

uint8_t I2CModule;
static const I2C_MemMapPtr I2C_BASE_TABLE[2] = I2C_BASE_PTRS;
static const IRQn_Type I2C_IRQ_TABLE[2] = {I2C0_IRQn,I2C1_IRQn};

uint8_t adlx345_write1(uint8_t No, uint8_t slaveAddress, uint8_t Data,uint16_t command);
void adxl345_start(uint8_t No);
void adxl345_write(uint8_t No,uint8_t data);
uint8_t adxl345_wait(uint8_t No);
uint8_t adxl345_get_ack(uint8_t No);
void adxl345_stop(uint8_t No);
//=======================�ӿں���=======================================
//======================================================================
//�������ƣ�adlx345_init
//����������range:������Χ��rate:�������ʣ�power:��Դģʽ��dataready:
//         DATA_READY�ж����ã�xOffset:X��ƫ������yOffset:Y��ƫ������
//         zOffset:Z��ƫ������
//�������أ���
//���ܸ�Ҫ��ADLX345���ٶȴ�������ʼ����
//======================================================================
void adlx345_init(uint8_t module,uint8_t range,uint8_t rate,uint8_t power,uint8_t dataready,
		uint8_t xOffset,uint8_t yOffset,uint8_t zOffset)
{
	I2CModule=module;
	i2c_init(I2CModule,1,0X9F,100);    //I2Cģ���ʼ��
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
//�������ƣ�adlx345_read1
//����������command:��ȡ��ַ�������data:��Ŷ�ȡ������
//�������أ���
//���ܸ�Ҫ��ADLX345���ٶȴ�������ȡ1�ֽڡ�
//======================================================================
void adlx345_read1(uint8_t command,uint8_t *data)
{
	i2c_read1(I2CModule,slaveaddress,command,data);
}

//======================================================================
//�������ƣ�adlx345_readN
//����������command:��ȡ��ַ�������data:��Ŷ�ȡ������;num:�ֽ���
//�������أ���
//���ܸ�Ҫ��ADLX345���ٶȴ�������ȡN�ֽڡ���
//======================================================================
void adlx345_readN(uint8_t command,uint8_t *data,uint8_t num)
{
	i2c_readN(I2CModule,slaveaddress,command,data,num);
}
//================================�ڲ�����===============================
//=====================================================================
//�������ƣ� i2c_write1
//���ܸ�Ҫ�� ����1���ֽ�����
//����˵���� No:ģ��ţ���ȡֵΪ0��1
//          slaveAddress������Ŀ��ĵ�ַ
//          Data:�����͵ĵ�1���ֽ�����
//          command:��ַ�����
//�������أ�  0���ɹ�����һ���ֽڣ�1������һ���ֽ�ʧ��
//����˵��:  1����ʾ����ʧ�ܣ�0�����ͳɹ�
//=====================================================================
uint8_t adlx345_write1(uint8_t No, uint8_t slaveAddress, uint8_t Data,uint16_t command)
{
	adxl345_start(No);               //���Ϳ�ʼ�ź�
	slaveAddress = (slaveAddress << 1) & 0xfe;
	adxl345_write(No,slaveAddress);  //����ַ�ͷ���λ���͸��ӻ�
	if(adxl345_wait(No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	if(adxl345_get_ack(No) == 1)     //�ж��Ƿ���ACK
		return 1;                    //���Ƿ�Ӧ���ź��򷵻�ʧ��
	if(0xFFFF != command)
	{
		adxl345_write(No,command);  //����ַ�ͷ���λ���͸��ӻ�
		if(adxl345_wait(No) == 1)        //�ȴ����
			return 1;                    //ʧ��
		if(adxl345_get_ack(No) == 1)     //�ж��Ƿ���ACK
			return 1;
	}
	adxl345_write(No,Data);          //����ַ�ͷ���λ���͸��ӻ�
	if(adxl345_wait(No) == 1)        //�ȴ����
		return 1;                    //ʧ��
	adxl345_stop(No);                //����ֹͣ�ź�
}

//=====================================================================
//�������ƣ� adxl345_start
//���ܸ�Ҫ�� ��ʼ�ź�
//����˵���� No:ģ���,��ȡֵΪ0,1
//�������أ� ��
//=====================================================================
 void adxl345_start(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BSET(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //��λ����ģʽ
	BSET(I2C_C1_MST_SHIFT,i2c_ptr->C1); //�������ĳɴӻ�����ֹͣ�ź�
}

 //=====================================================================
 //�������ƣ� adxl345_write
 //���ܸ�Ҫ�� ��������
 //����˵���� No:ģ���,��ȡֵΪ0,1
 //        data��Ҫд�������
 //�������أ� ��
 //=====================================================================
 void adxl345_write(uint8_t No,uint8_t data)
 {
 	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
 	i2c_ptr->D = data;
 }

 //=====================================================================
 //�������ƣ�wait
 //���ܸ�Ҫ���ȴ����
 //����˵����No:ģ���,��ȡֵΪ0,1
 //�������أ�1����ʾ��ʱ��0����ʾ����
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
//�������ƣ� adxl_get_ack
//���ܸ�Ҫ�� ��ȡӦ����߷�Ӧ���ź�
//����˵���� No:ģ���,��ȡֵΪ0,1
//�������أ�0��Ӧ���źţ�1����Ӧ���ź�
//=====================================================================
uint8_t adxl345_get_ack(uint8_t No)
{
    I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
    uint8_t ack;
    ack = BGET(I2C_S_RXAK_SHIFT,i2c_ptr->S);
    return ack;
}

//=====================================================================
//�������ƣ� adxl345_stop
//���ܸ�Ҫ�� ֹͣ�ź�
//����˵���� No:ģ���,��ȡֵΪ0,1
//�������أ� ��
//=====================================================================
void adxl345_stop(uint8_t No)
{
	I2C_MemMapPtr i2c_ptr = I2C_BASE_TABLE[No];
	BCLR(I2C_C1_MST_SHIFT,i2c_ptr->C1); //�������ĳɴӻ�����ֹͣ�ź�
	BCLR(I2C_C1_TX_SHIFT,i2c_ptr->C1);  //��λ����ģʽ
}
void adlx345_Dly_ms(uint32_t ms)
{
    for(uint32_t i= 0; i < (6000*ms); i++) __asm("nop");
}

