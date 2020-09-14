//=====================================================================
//�ļ����ƣ�timer.c
//���ܸ�Ҫ��timer�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��KL25��KL26��KL36
//=====================================================================
#include "timer.h"

void lptmr_init(uint32_t time_ms);
void lptmr_reset(void);
void lptmr_enable_int(void);
void lptmr_disable_int(void);
void tpm_timer_init(uint16_t TPM_i,uint32_t f,float MOD_Value);
void tpm_enable_int(uint8_t TPM_i);
void tpm_disable_int(uint8_t TPM_i);
uint8_t tpm_get_int(uint8_t TPM_i);
void tpm_clear_int(uint8_t TPM_i);

//���˿ڻ���ַ���볣��������PORT_ARR[0]~PORT_ARR[4]��
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//��ʱ��ģ��0,1,2��ַӳ��
TPM_MemMapPtr TPM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};
IRQn_Type TPM_IRQ[]={TPM0_IRQn,TPM1_IRQn,TPM2_IRQn};

//============================================================================
//�������أ���
//����˵���� timer_No:ʱ��ģ���  ����ʱ��ģ��ż�gec.h�ļ�����
//        time_ms ��ʱ���жϵ�ʱ��������λΪ���룬����timer0���÷�Χ 0~65535ms��
//		  timer1��timer2��ʱʱ����ΧΪ0~65535/3ms
//���ܸ�Ҫ��ʱ��ģ���ʼ��������timer0Ϊ�͹���ʱ�� timer1��timer2Ϊ�ǵ͹���ʱ��
//============================================================================
void timer_init(uint8_t timer_No,uint32_t time_ms)
{
    if(timer_No==0)
    {
        lptmr_init(time_ms);
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_timer_init(timer_No,3000,time_ms);
    }
    else
        return;
}


//============================================================================
//�������أ���
//����˵���� timer_No:ʱ��ģ���  ����ʱ��ģ��ż�gec.h�ļ�����
//���ܸ�Ҫ��ʱ��ģ��ʹ�ܣ�����ʱ��ģ���жϼ���ʱ���ж�
//============================================================================
void timer_enable_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
        lptmr_enable_int();
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_enable_int(timer_No);
    }
    else
        return;
}

//============================================================================
//�������أ���
//����˵���� timer_No:ʱ��ģ���  ����ʱ��ģ��ż�gec.h�ļ�����
//���ܸ�Ҫ����ʱ���жϳ���
//============================================================================
void timer_disable_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
        lptmr_disable_int();
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_disable_int(timer_No);
    }
    else
        return;
}

//===================================================================
//����˵���� timer_No:ʱ��ģ���  ����ʱ��ģ��ż�gec.h�ļ�����
//���ܸ�Ҫ����ȡtimerģ���жϱ�־
//�������أ��жϱ�־ 1=�ж�Ӧģ���жϲ���;0=�޶�Ӧģ���жϲ���
//===================================================================
uint8_t timer_get_int(uint8_t timer_No)
{

    if(timer_No==0)
    {
      //��ȡTPM_iģ���жϱ�־λ
      if((LPTMR0_CSR & LPTMR_CSR_TCF_MASK)==LPTMR_CSR_TCF_MASK)
          return 1;
      else
          return 0;
    }
    else if(timer_No==1||timer_No==2)
    {
        return tpm_get_int(timer_No);
    }
    else
        return 0;
}

//============================================================================
//�������أ���
//����˵���� timer_No:ʱ��ģ���  ����ʱ��ģ��ż�gec.h�ļ�����
//���ܸ�Ҫ����ʱ������жϱ�־
//============================================================================
void timer_clear_int(uint8_t timer_No)
{
    if(timer_No==0)
    {
        CLEAR_LPTMR_FLAG;
    }
    else if(timer_No==1||timer_No==2)
    {
        tpm_clear_int(timer_No);
    }
    else
        return;
}

//�ڲ�����
//============================================================================
//�������أ���
//����˵����time_ms ��ʱ���жϵ�ʱ��������λΪ���룬ʱ�����÷�Χ 0~65535ms
//���ܸ�Ҫ��LPTMRģ���ʼ����ʱ��Դ������Ϊ1KHz��LPOʱ��
//�����Դ�����
//============================================================================
void lptmr_init(uint32_t time_ms)
{
    uint16_t compare_value;
    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;//ʹ��LPTMRģ��ʱ��
    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0x11);//ѡ��LPOʱ��
    LPTMR0_PSR |= LPTMR_PSR_PCS(1)|LPTMR_PSR_PBYP_MASK;
    compare_value=time_ms;
    LPTMR0_CMR = LPTMR_CMR_COMPARE(compare_value);      //���ñȽϼĴ���ֵ
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; //����LPTMRģ������
}


//====================�ڲ�����==========================================
//============================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��LPTMR��ʱ������
//============================================================================
void lptmr_reset(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TCF_MASK;
}

//============================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��LPTMRģ��ʹ�ܣ�����LPTMRģ���жϼ���ʱ���ж�
//============================================================================
void lptmr_enable_int(void)
{
    LPTMR0_CSR|=LPTMR_CSR_TIE_MASK;        //����LPTMR��ʱ���ж�
    NVIC_EnableIRQ(LPTMR0_IRQn);           //�����ŵ�IRQ�ж�
}

//============================================================================
//�������أ���
//����˵������
//���ܸ�Ҫ��LPT��ʱ���жϳ���
//============================================================================
void lptmr_disable_int(void)
{
    LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;      //��ֹLPTMR��ʱ���ж�
    NVIC_DisableIRQ(LPTMR0_IRQn);          //�����ŵ�IRQ�ж�
}

//===================================================================
//���ܸ�Ҫ�� tpmģ���ʼ�������ü�����Ƶ��f���������������ʱ��MOD_Value��
//����˵���� TPM_i��ģ��ţ����ò����ɲμ�gec.h�ļ�
//        f��������Ƶ�ʣ���λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//        MOD_Value���������������ʱ�䣬��λ��ms��(f*MOD_Value-1)�ķ�ΧΪ0~65535
//�������أ� ��
//===================================================================
void tpm_timer_init(uint16_t TPM_i,uint32_t f,float MOD_Value)
{
    //�ֲ���������
    uint32_t clk_f,clk_div;
    uint16_t mod;
    //1. ����SIMʱ����
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    TPM_ARR[TPM_i]->SC = 0;   //�رո�ģ��ļ��������ܣ�ȷ��δ�ظ�����
    //2.����ʱ�ӣ�Ĭ��ѡ����PLL/2ʱ��Դ����48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    //3. ��������ʱ��Ƶ��f�������Ƶ����clk_div�����Ƶϵ��clk_f=48000/f,
    //���Ƶ����clk_div=sqrt(clk_f)�����磺f=3000Khz,��clk_f=16��clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //4. ����������
    TPM_ARR[TPM_i]->CNT = 0;
    //5. ����ģ���Ĵ���
    if(f * MOD_Value > 65535)
    {
        mod = 65535;
    }
    else if(f * MOD_Value <= 0)
    {
        mod = 0 ;
    }
    else{
        mod = f * MOD_Value - 1;
    }
    TPM_ARR[TPM_i]->MOD = mod;  //��ģ���Ĵ�����ֵ
    //6. дTPM_i��״̬�Ϳ��ƼĴ���
    TPM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(clk_div) | TPM_SC_TOIE_MASK;
}

//===================================================================
//���ܸ�Ҫ��ʹ��tpmģ���жϡ�
//����˵����TPM_i��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ���
//===================================================================
void tpm_enable_int(uint8_t TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //��TPM�ж�
    NVIC_EnableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//���ܸ�Ҫ������tpmģ���жϡ�
//����˵����TPM_i��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ���
//===================================================================
void tpm_disable_int(uint8_t TPM_i)
{
    if(TPM_i>2)  TPM_i = 2;
    //��TPM�ж�
    NVIC_DisableIRQ(TPM_IRQ[TPM_i]);
}

//===================================================================
//���ܸ�Ҫ����ȡTPMģ���жϱ�־
//����˵����TPM_i��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ��жϱ�־ 1=�ж�Ӧģ���жϲ���;0=�޶�Ӧģ���жϲ���
//===================================================================
uint8_t tpm_get_int(uint8_t TPM_i)
{
  //��ȡTPM_iģ���жϱ�־λ
  if(((TPM_SC_REG(TPM_ARR[TPM_i]) & TPM_SC_TOF_MASK)==TPM_SC_TOF_MASK))
    return 1;
  else
    return 0;
}

//===================================================================
//���ܸ�Ҫ�����TPM�жϱ�־
//����˵����TPM_i��ģ��ţ����ò����ɲμ�gec.h�ļ�
//�������أ����TPMģ���жϱ�־λ
//===================================================================
void tpm_clear_int(uint8_t TPM_i)
{
    //���TPM_iģ���жϱ�־λ
    BSET(TPM_SC_TOF_SHIFT,TPM_SC_REG(TPM_ARR[TPM_i]));
}
