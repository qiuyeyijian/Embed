//=====================================================================
//�ļ����ƣ�incapture.c
//���ܸ�Ҫ��incapture�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��KL25��KL26��KL36
//=====================================================================

#include "incapture.h"

//���˿ڻ���ַ���볣��������PORT_ARR[0]~PORT_ARR[4]��
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//��ʱ��ģ��0,1,2��ַӳ��
static const TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
static const IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

static uint8_t tpm_mux_val(uint16_t capNo,uint8_t* TPM_i,uint8_t* chl);

static void tpm_timer_init2(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value);

//===================================================================
//�������ƣ�incap_init
//���ܸ�Ҫ��incapģ���ʼ����
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//        clockFre��ʱ��Ƶ�ʣ���λ��Khz��ȡֵ��375��750��1500��3000��6000��
//                                                   12000��24000��48000
//        period�����ڣ���λΪ��������������������������ΧΪ1~65536
//        capmode�����벶׽ģʽ�������ء��½��ء�˫���أ����к궨�峣��ʹ��
//�������أ���
//ע�⣺          ��ΪGEC�и�����PWM�����벶׽����ͬһģ��ģ�ֻ��ͨ����ͬ������Ϊ��ֹ��ʹ�ö���
//          PWM�����벶׽ʱ��Ƶ�ʴ۸ģ���Ҫʹ��ʹ�õ���clockFre��period��������һ�¡�
//===================================================================
void incapture_init(uint16_t capNo,uint32_t clockFre,uint16_t period,uint8_t capmode)
{
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint8_t port,pin;           //�������Ķ˿ڡ����ź���ʱ����
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint8_t mux_val;
    //1. gpio���Ž���
    port = (capNo>>8);     //�������Ķ˿�
    pin = capNo;           //�����������ź�
    //2. ����port�����ֲ�����port_ptr��ֵ,��û���ַ
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    //3. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    mux_val=tpm_mux_val(capNo,&TPM_i,&chl);
    //4. ��ʼ��ʱ��
    tpm_timer_init2(TPM_i,clockFre,period);
    //4. ����pin,ָ�������Ź���ΪTPM��ͨ�����ܣ��������ſ��ƼĴ�����MUX=mux_val��
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX(mux_val);
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. ���벶׽�����趨
    if(capmode == CAP_UP)           //�����ز�׽
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK;
    else if(capmode == CAP_DOWN)    //�½��ز�׽
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSB_MASK;
    else if(capmode == CAP_DOUBLE)  //˫���ز�׽
    {
        TPM_CnSC_REG(TPM_ARR[TPM_i],chl) = TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK;
    }
    //6. chlͨ���ж�ʹ��
    TPM_CnSC_REG(TPM_ARR[TPM_i],chl) |= TPM_CnSC_CHIE_MASK;
    TPM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1);
}

//=====================================================================
//�������ƣ�incapture_value
//���ܸ�Ҫ����ȡ��ͨ���ļ�������ǰֵ
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//�������أ�ͨ���ļ�������ǰֵ
//=====================================================================
uint16_t get_incapture_value(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint16_t cnt;
    tpm_mux_val(capNo,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    cnt=TPM_CnV_REG(TPM_ARR[TPM_i],chl);
    return cnt;
}

//===================================================================
//�������ƣ�cap_clear_flag
//���ܸ�Ҫ�������벶׽�жϱ�־λ��
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//�������أ���
//===================================================================
void cap_clear_flag(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    tpm_mux_val(capNo,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    BSET(TPM_CnSC_CHF_SHIFT,TPM_ARR[TPM_i]->CONTROLS[chl].CnSC);
}

//===================================================================
//�������ƣ�cap_get_flag
//���ܸ�Ҫ����ȡ���벶׽�жϱ�־λ��
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//�������أ����ص�ǰ�жϱ�־λ
//===================================================================
uint8_t cap_get_flag(uint16_t capNo)
{
    uint8_t TPM_i,chl,flag;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    tpm_mux_val(capNo,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    flag = BGET(TPM_CnSC_CHF_SHIFT,TPM_ARR[TPM_i]->CONTROLS[chl].CnSC);
    return flag;
}

//===================================================================
//�������ƣ�cap_enable_int
//���ܸ�Ҫ��ʹ�����벶׽�жϡ�
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//�������أ���
//===================================================================
void cap_enable_int(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    tpm_mux_val(capNo,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//�������ƣ�cap_disable_int
//���ܸ�Ҫ����ֹ���벶׽�жϡ�
//����˵����capNo��ģ��ţ�ʹ��gec.h�к궨�壬����INCAP_PIN0
//�������أ���
//===================================================================
void cap_disable_int(uint16_t capNo)
{
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    tpm_mux_val(capNo,&TPM_i,&chl);   //����tpmģ��ź�ͨ����
    DisableIRQ(TPM_IRQ[TPM_i]);
}

//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�tpm_mux_val
//���ܸ�Ҫ������������tpmx_Chy���н������ó�����ģ�����ͨ���ţ�����TPM_CH0
//        ������PORT���ţ����������ŷ���mux_val����
//����˵����tpmx_Chy��ģ��ͨ���ţ�����TPM_CH0��ʾΪTPM0ģ���0ͨ����
//
//�������أ�gpio���üĴ�������ֵ
//=====================================================================
static uint8_t tpm_mux_val(uint16_t capNo,uint8_t* TPM_i,uint8_t* chl)
{
    uint8_t port,pin;
    //1.����ģ��ź�ͨ����
    switch(capNo)
    {
        case ((2<<8)|1):*TPM_i =0;*chl=0;break;
        case ((2<<8)|2):*TPM_i =0;*chl=1;break;
        case ((2<<8)|3):*TPM_i =0;*chl=2;break;
        case ((3<<8)|3):*TPM_i =0;*chl=3;break;
        case ((3<<8)|4):*TPM_i =0;*chl=4;break;
        case ((3<<8)|5):*TPM_i =0;*chl=5;break;
        default:break;
    }
    //2.�������Ÿ��üĴ�������ֵ
    port = (capNo>>8);
    pin = capNo;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}

//===================================================================
//�������ƣ� tpm_timer_init
//���ܸ�Ҫ�� tpmģ���ʼ�������ü�����Ƶ��f�����������ʱ��MOD_Value��
//����˵���� TPM_i��ģ��ţ�ʹ�ú궨�壺TPM_0��TPM_1��TPM_2
//        f����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//        MOD_Value����λ��ms����Χȡ���ڼ�����Ƶ���������λ����16λ��
//�������أ� ��
//===================================================================
static void tpm_timer_init2(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value)
{
    //�ֲ���������
    uint32_t clk_f,clk_div;
    //��1������SIMʱ����
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //��2������ʱ�ӣ�Ĭ��ѡ����PLL/2ʱ��Դ����48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    TPM_ARR[TPM_i]->SC = 0;
    //��3�� ��������ʱ��Ƶ��f�������Ƶ����clk_div�����Ƶϵ��clk_f=48000/f,
    //���Ƶ����clk_div=sqrt(clk_f)�����磺f=3000Khz,��clk_f=16��clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //��4�� ����������
    TPM_ARR[TPM_i]->CNT = 0;
    //��5������ģ���Ĵ���
    if(MOD_Value == 0)
    {
    	TPM_ARR[TPM_i]->MOD = 0;  //��ģ���Ĵ�����ֵ
    }
    else
    {
    	TPM_ARR[TPM_i]->MOD = MOD_Value - 1;  //��ģ���Ĵ�����ֵ
    }
    //��6��дTPM_i��״̬�Ϳ��ƼĴ���
    TPM_ARR[TPM_i]->SC |= TPM_SC_PS(clk_div);
}

