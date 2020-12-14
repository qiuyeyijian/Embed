//=====================================================================
//�ļ����ƣ�spi.c
//���ܸ�Ҫ��spi�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��KL25��KL26��KL36
//=====================================================================
#include "spi.h"


//=====================================================================
//����˵����SPI��ʼ��
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//       MSTR��SPI���ӻ�ѡ��0ѡ��Ϊ�ӻ�,1ѡ��Ϊ������
//       BaudRate�������ʣ���ȡ12000��6000��4000��3000��1500��1000��
//       ��λ��bps
//       CPOL��CPOL=0������ЧSPIʱ�ӣ�����Ч����
//       CPOL=1������ЧSPIʱ�ӣ�����Ч��
//       CPHA��CPHA=0��λΪ0�� CPHA=1��λΪ1��
//�������أ���
//=====================================================================
void spi_init(uint8_t No, uint8_t MSTR, uint16_t BaudRate, \
    uint8_t CPOL, uint8_t CPHA)
{
    uint8_t BaudRate_Mode;
    uint8_t BaudRate_High;
    uint8_t BaudRate_Low;
    //���SPI�Ų���������ǿ��ѡ�� 0��ģ��
    if (No<SPI_0 || No>SPI_1)   No = SPI_0;
    if (No == SPI_0)                         //��ʼ��SPI0����
    {
        BSET(SIM_SCGC4_SPI0_SHIFT, SIM_SCGC4);   //��SPI0ģ��ʱ�ӡ�
        //���Ÿ���ΪSPI0����
        PORTC_PCR4 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTC4��SS����
        PORTC_PCR5 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTC5��SCK����
        PORTC_PCR6 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTC6��MOSI����
        PORTC_PCR7 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTC7��MIOS����

        SPI0_C1 = 0x00;                           //SPI���ƼĴ���1����
        BSET(SPI_C1_SPE_SHIFT, SPI0_C1);         //ʹ��SPIģ��

        //MSTR=1Ϊ����ģʽ��
        //MSTR=0Ϊ�ӻ�ģʽ����MSTR��ʼֵΪ0��������ģ�
        (MSTR == 1) ? BSET(SPI_C1_MSTR_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_SPIE_SHIFT, SPI0_C1);

        //ʱ�Ӽ������ã�CPOL=0��ƽʱʱ��Ϊ�ߵ�ƽ����֮CPOL=1��ƽʱʱ��Ϊ�͵�ƽ
        (0 == CPOL) ? BCLR(SPI_C1_CPOL_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_CPOL_SHIFT, SPI0_C1);

        //ʱ����λCPHA
        (0 == CPHA) ? BCLR(SPI_C1_CPHA_SHIFT, SPI0_C1) : \
            BSET(SPI_C1_CPHA_SHIFT, SPI0_C1);

        //SSOEΪ1,MODFENΪ1�����ñ�ģ��Ϊ�Զ�SS���
        BSET(SPI_C1_SSOE_SHIFT, SPI0_C1);
        //��SPI0��C1�Ĵ�������Ϊ����ģʽ���ӻ�ѡ���Զ�ģʽ��ʹ��SPI0ģ�顣
        SPI0_C2 = 0x00;
        if (MSTR == 1)  //����ģʽ
            BSET(SPI_C2_MODFEN_SHIFT, SPI0_C2);
        SPI0_BR = 0x00U;//�����ʼĴ�������
        //�������ò�����
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
            SPI0_BR = BaudRate_High << 4;//��ֵ����SPI0_BR��SPPR��D6D5D4λ
            SPI0_BR |= BaudRate_Low;//��ֵ��SPI0_BR��SPR�� D2D1D0λ
        }
    }
    else         //��ʼ��SPI1����
    {
        BSET(SIM_SCGC4_SPI1_SHIFT, SIM_SCGC4);   //��SPI1ģ��ʱ�ӡ�
        //���Ÿ���ΪSPI1����
        PORTD_PCR4 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTD4��SS����
        PORTD_PCR5 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTD5��SCK����
        PORTD_PCR6 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTD6��MOSI����
        PORTD_PCR7 = (0 | PORT_PCR_MUX(0x02));     //ѡ��PTD7��MIOS����

        SPI1_C1 = 0x00;                           //SPI���ƼĴ���1����
        BSET(SPI_C1_SPE_SHIFT, SPI1_C1);         //ʹ��SPIģ��
        //MSTR=1Ϊ����ģʽ��
        //MSTR=0Ϊ�ӻ�ģʽ����MSTR��ʼֵΪ0��������ģ�
        (MSTR == 1) ? BSET(SPI_C1_MSTR_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_SPIE_SHIFT, SPI1_C1);

        //ʱ�Ӽ������ã�CPOL=0��ƽʱʱ��Ϊ�ߵ�ƽ����֮CPOL=1��ƽʱʱ��Ϊ�͵�ƽ
        (0 == CPOL) ? BCLR(SPI_C1_CPOL_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_CPOL_SHIFT, SPI1_C1);

        //CPHA=0ʱ���źŵĵ�һ�����س�����8�������ݴ���ĵ�һ�����ڵ����룻
        //CPHA=1ʱ���źŵĵ�һ�����س�����8�������ݴ���ĵ�һ�����ڵ���㡣
        (0 == CPHA) ? BCLR(SPI_C1_CPHA_SHIFT, SPI1_C1) : \
            BSET(SPI_C1_CPHA_SHIFT, SPI1_C1);
        //SSOEΪ1,MODFENΪ1�����ñ�ģ��Ϊ�Զ�SS���
        BSET(SPI_C1_SSOE_SHIFT, SPI1_C1);
        //��SPI0��C1�Ĵ�������Ϊ����ģʽ���ӻ�ѡ���Զ�ģʽ��ʹ��SPI0ģ�顣

        //BSET(SPI_C1_SPIE_SHIFT,SPI1_C1);  //����ģ���SPI�ж�
        SPI1_C2 = 0x00U;
        if (MSTR == 1)  //����ģʽ
            BSET(SPI_C2_MODFEN_SHIFT, SPI1_C2);
        SPI1_BR = 0x00U;
        //�������ò�����
        BaudRate_High = 0;
        BaudRate_Low = 0;
        BaudRate_Mode = 24000 / BaudRate;   //ȡ�������ڼĴ��������ݼ���
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
            SPI0_BR = BaudRate_High << 4;//��ֵ����SPI0_BR��SPPR��D6D5D4λ
            SPI0_BR |= BaudRate_Low;   //��ֵ��SPI0_BR��SPR�� D2D1D0λ
        }
    }
}

//=====================================================================
//����˵����SPI����һ�ֽ����ݡ�
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//       data��     ��Ҫ���͵�һ�ֽ����ݡ�
//�������أ�0������ʧ�ܣ�1�����ͳɹ���
//=====================================================================
uint8_t spi_send1(uint8_t No, uint8_t data)
{
    // ��λ�󣬷��ͻ�������־λΪ1; 0��ʾ����1��ʾ��
    // �������ݼĴ���д���ݺ��Զ������ͻ�������־λ��1
    SPI_MemMapPtr baseadd = SPI_baseadd(No);

    //�ȴ���һ���ַ�������ϣ����ͻ���������
    while (!(SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK));

    // ��Ҫ���͵�����д�뵽���ݼĴ���
    // �����ݼĴ���д�����ݽ���д�뵽�������ݻ������������ݷ��ͳ�ȥ
    // �����ݼĴ��������ȡ���������ݼĴ�����ֵ��
    SPI_DL_REG(baseadd) = data;

    for (uint32_t i = 0;i < 65535;i++) {
        // ��һ��ʱ����������ͻ�����Ϊ�ձ�־��� 1��˵�����ͻ�����Ϊ�գ��������
        if (SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK) {
            return(1);
        }
    }

    //��һ��ʱ������Ȼû�з���1,����Ϊ����ʧ�ܣ�����0��
    return(0);
}

//=====================================================================
//����˵����SPI�������ݡ�
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//       n:     Ҫ���͵��ֽڸ�������ΧΪ(1~255)
//       data[]:����������׵�ַ��
//�������أ��ޡ�
//=====================================================================
void spi_sendN(uint8_t No, uint8_t n, uint8_t* buff) {

    // // ��һ��ʵ�ַ�ʽ
    // SPI_MemMapPtr baseadd = SPI_baseadd(No);
    // for (uint32_t k = 0;k < n;k++) {
    //     //�ȴ���һ���ַ�������ϣ����ͻ���������
    //     while (!(SPI_S_REG(baseadd) & SPI_S_SPTEF_MASK));
    //     SPI_DL_REG(baseadd) = data[k];

    //     // �˴��о�����Ҫ��SPTEFλ��Ҳ���ǽ�SPTEF��1
    //     // ��Ϊ�����ݼĴ���д����֮�󣬻��Զ���SPTEF��1
    //     SPI_S_REG(baseadd) |= SPI_S_SPTEF_MASK;    //���SPTEFλ
    // }

    // �ڶ���ʵ�ַ�ʽ��ͨ������spi_send1()Ҳ����ʵ����������
    for (uint32_t i = 0; i < n; i++) {
        spi_send1(No, buff[i]);
    }

    // ����ת���ַ�0, ���߽��շ�һ���ַ����������
    spi_send1(No, '\0');
}

//=====================================================================
//����˵����SPI����һ���ֽڵ�����
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ����յ������ݡ�
//=====================================================================
uint8_t spi_receive1(uint8_t No)
{
    SPI_MemMapPtr baseadd = SPI_baseadd(No);

    // ���ջ���Ĵ�������־��SPRF��
    // SPRF = 0, ���ջ���Ĵ����޿������ݣ�SPRF = 1, ���ջ���Ĵ����п�������
    // ��ȡ���ջ���Ĵ���֮�󣬸ñ�־����0
    while (!(SPI_S_REG(baseadd) & SPI_S_SPRF_MASK)); // ���п�������ʱ������whileѭ��

    return SPI_DL_REG(baseadd);
}

//=====================================================================
//����˵����SPI�������ݡ���n=1ʱ�����ǽ���һ���ֽڵ����ݡ���
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//        n:    Ҫ���͵��ֽڸ�������ΧΪ(1~255),
//       data[]:���յ������ݴ�ŵ��׵�ַ��
//�������أ�1�����ճɹ�,���������ʧ�ܡ�
//=====================================================================

uint32_t spi_receiveN(uint8_t No, uint8_t* buff) {
    // ����һ����̬��������¼��ǰ�����ַ�����
    static uint32_t index = 0;

    buff[index++] = spi_receive1(No);

    // �����յ�ת���ַ�'\0'ʱ�� ˵�����ͷ��Ѿ��������
    // ��ʱ���ؽ��յ��ַ����ĳ��ȣ�ͬʱindex��0, ���������һ���ַ���
    if (buff[index - 1] == '\0') {
        index = 0;
        return strlen(buff);
    }

    // ��������ַ�'\0', ˵���������ݣ����� 0
    return 0;
}


//=====================================================================
//����˵������SPI�����жϡ�
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ��ޡ�
//=====================================================================
void spi_enable_re_int(uint8_t No)
{
    //enable_irq (No+10);
    NVIC_EnableIRQ(No + 10);
}

//=====================================================================
//����˵�����ر�SPI�����жϡ�
//����������No��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ��ޡ�
//=====================================================================
void spi_disable_re_int(uint8_t No)
{
    //disable_irq (No+10);
    NVIC_DisableIRQ(No + 10);
}
