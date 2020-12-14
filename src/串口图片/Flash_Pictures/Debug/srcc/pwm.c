//=====================================================================
//�ļ����ƣ�pwm.c
//���ܸ�Ҫ��pwm�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��KL25��KL26��KL36
//=====================================================================
#include "pwm.h"

//���˿ڻ���ַ���볣��������PORT_ARR[0]~PORT_ARR[4]��
static const PORT_MemMapPtr PORT_ARR[]={PORTA_BASE_PTR,PORTB_BASE_PTR,
                         PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//��ʱ��ģ��0,1,2��ַӳ��
TPM_MemMapPtr PWM_ARR[]={TPM0_BASE_PTR,TPM1_BASE_PTR,TPM2_BASE_PTR};

//*****************************�ڲ�����******************************
static uint8_t tpm_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl);
static void tpm_timer_init1(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value);

//*****************************����ӿں���******************************
//=====================================================================
//�������ƣ�  pwm_init
//���ܸ�Ҫ��  pwm��ʼ������
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          clockFre��ʱ��Ƶ�ʣ���λ��Khz��ȡֵ��375��750��1500��3000��6000��
//                                                   12000��24000��48000
//          period�����ڣ���λΪ��������������������������ΧΪ1~65536
//          duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//          align�����뷽ʽ ����ͷ�ļ��궨���������PWM_EDGEΪ���ض��䡣
//          pol�����ԣ���ͷ�ļ��궨���������PWM_PLUSΪ������
//�������أ�  ��
//ע�⣺          ��ΪGEC�и�����PWM�����벶׽����ͬһģ��ģ�ֻ��ͨ����ͬ������Ϊ��ֹ��ʹ�ö���
//          PWM�����벶׽ʱ��Ƶ�ʴ۸ģ���Ҫʹ��ʹ�õ���clockFre��period��������һ�¡�
//     ռ�ձȣ���ָ�ߵ�ƽ��һ����������ռ��ʱ�������
//=====================================================================
void pwm_init(uint16_t pwmNo,uint32_t clockFre,uint16_t period,float duty,
		                                        uint8_t align,uint8_t pol)
{
    PORT_MemMapPtr port_ptr;   //����port_ptrΪPORT�ṹ������ָ��
    uint8_t port,pin;           //�������Ķ˿ڡ����ź���ʱ����
    uint8_t mux_val;            //���ڴ�Ż�ȡ�ĸ�����
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint32_t temp;
    // ��ֹԽ��
    if(duty>100.0) duty=100.0;
    //��1��gpio���Ž���
    port = (pwmNo>>8);          //�������Ķ˿�
    pin = pwmNo;                //�����������ź�
    port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
    //��2��ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    mux_val=tpm_mux_val(pwmNo,&TPM_i,&chl);
    //��3������pin,ָ�������Ź���ΪTPM��ͨ�����ܣ��������ſ��ƼĴ�����MUX=mux_val��
    PORT_PCR_REG(port_ptr, pin) &= ~PORT_PCR_MUX_MASK;
    PORT_PCR_REG(port_ptr, pin) |= PORT_PCR_MUX(mux_val);
    //5. PWM���뷽ʽ���趨
    if(align == PWM_CENTER)         //���Ķ���
    {
        tpm_timer_init1(TPM_i,clockFre,period / 2);
    	PWM_ARR[TPM_i]->SC |= TPM_SC_CPWMS_MASK;   //�������Ķ���
        TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0;
        //6. PWM���Ե��趨��PWMռ�ձȵ��趨
        if(pol == PWM_PLUS)            //������
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSB_MASK;
            temp = (uint32_t)(duty*period/200);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
        else                            //������
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSA_MASK;
            temp = (uint32_t)(period*(100-duty)/200);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
    }
    else                            //���ض���
    {
        tpm_timer_init1(TPM_i,clockFre,period);
    	PWM_ARR[TPM_i]->SC &= ~TPM_SC_CPWMS_MASK; //���ñ��ض���
    	TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0;     //��ͨ�����ƼĴ���
        //6. PWM���Ե��趨��PWMռ�ձȵ��趨
        temp = PWM_ARR[TPM_i]->MOD + 1;    //�������ڣ�period��
        if(pol == PWM_PLUS)             //������
        {
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSB_MASK;
            temp = (uint32_t)(temp*duty/100);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
        else                            //������
        {
        	TPM_CnSC_REG(PWM_ARR[TPM_i],chl) = 0; //��ͨ������״̬�Ĵ���
        	//���ø�����
            TPM_CnSC_REG(PWM_ARR[TPM_i],chl)=TPM_CnSC_MSB_MASK| \
                                             TPM_CnSC_ELSA_MASK;
            //����ռ�ձ�
            temp = (uint32_t)(temp*(100-duty)/100);
            if(temp >= 65536)
            	TPM_CnV_REG(PWM_ARR[TPM_i],chl) = 65535;
            else
                TPM_CnV_REG(PWM_ARR[TPM_i],chl)=temp;
        }
    }
    PWM_ARR[TPM_i]->SC |= TPM_SC_CMOD(1);//ʹ�ܼ�����
}

//=====================================================================
//�������ƣ�  pwm_update
//���ܸ�Ҫ��  tpmxģ��Chyͨ����PWM����
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          duty��ռ�ձȣ�0.0~100.0��Ӧ0%~100%
//�������أ�  ��
//=====================================================================
void pwm_update(uint16_t pwmNo,float duty)
{
    uint8_t TPM_i,chl;  //��tpmx_Chy��������tpmģ��š�ͨ������ʱ����
    uint32_t period;
    // ��ֹԽ��
    if(duty>100.0)  duty=100.0;
    //1. ȡ�����Ÿ���ֵ������ý�����tpmģ��ź�ͨ����
    tpm_mux_val(pwmNo,&TPM_i,&chl);
    //2. ����PWMͨ���Ĵ���ֵ
    period=PWM_ARR[TPM_i]->MOD;
    TPM_CnV_REG(PWM_ARR[TPM_i],chl)=(uint32_t)(period*duty/100);
}

//------����Ϊ�ڲ�����------
//=====================================================================
//�������ƣ�tpm_mux_val
//���ܸ�Ҫ������������pwmNo���н������ó�����ģ�����ͨ���ţ�����PWM_PIN0
//         ������PORT���ţ����������ŷ���mux_val����
//����˵����pwmNo��pwmģ��ţ���ͷ�ļ��ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//
//�������أ�gpio���üĴ�������ֵ
//=====================================================================
static uint8_t tpm_mux_val(uint16_t pwmNo,uint8_t* TPM_i,uint8_t* chl)
{
    uint8_t port,pin;
    //1.����ģ��ź�ͨ����
    switch(pwmNo)
    {
		case ((2<<8)|1):*TPM_i =0;*chl=0;break;
		case ((2<<8)|2):*TPM_i =0;*chl=1;break;
		case ((2<<8)|3):*TPM_i =0;*chl=2;break;
		case ((3<<8)|3):*TPM_i =0;*chl=3;break;
		case ((3<<8)|4):*TPM_i =0;*chl=4;break;
		case ((3<<8)|5):*TPM_i =0;*chl=5;break;
        default: break;
    }
    //2.�������Ÿ��üĴ�������ֵ
    port = (pwmNo>>8);
    pin = pwmNo;
    if(port<2 || port==4 || (port==2 && (pin==8||pin==9) ))
        return 3;
    else
        return 4;
}

//===================================================================
//�������ƣ�  tpm_timer_init
//���ܸ�Ҫ��  tpmģ���ʼ�������ü�����Ƶ��f�����������ʱ��MOD_Value��
//����˵����  pwmNo��pwmģ��ţ���gec.h�ĺ궨���������PWM_PIN0��ʾPWM0ͨ��
//          f����λ��Khz��ȡֵ��375��750��1500��3000��6000��12000��24000��48000
//          MOD_Value����λ��������Χȡ���ڼ�����Ƶ���������λ����16λ��
//�������أ�  ��
//===================================================================
static void tpm_timer_init1(uint16_t TPM_i,uint32_t f,uint16_t MOD_Value)
{
    //�ֲ���������
    uint32_t clk_f,clk_div;
    //��1�� ����SIMʱ����
    BSET(SIM_SCGC6_TPM0_SHIFT+TPM_i,SIM_SCGC6);
    //��2������ʱ�ӣ�Ĭ��ѡ����PLL/2ʱ��Դ����48MHz
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
    SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
    PWM_ARR[TPM_i]->SC = 0;
    //��3�� ��������ʱ��Ƶ��f�������Ƶ����clk_div�����Ƶϵ��clk_f=48000/f,
    //���Ƶ����clk_div=sqrt(clk_f)�����磺f=3000Khz,��clk_f=16��clk_div=4
    clk_f=48000/f;
    clk_div = 0;
    while(clk_f>1)
    {
        clk_div++;
        clk_f=clk_f/2;
    }
    //��4������������
    PWM_ARR[TPM_i]->CNT = 0;
    //��5������ģ���Ĵ���
    if(MOD_Value == 0)
    {
    	PWM_ARR[TPM_i]->MOD = 0;  //��ģ���Ĵ�����ֵ
    }
    else
    {
    	PWM_ARR[TPM_i]->MOD = MOD_Value - 1;  //��ģ���Ĵ�����ֵ
    }
    //��6��дTPM_i��״̬�Ϳ��ƼĴ���
    PWM_ARR[TPM_i]->SC |=  TPM_SC_PS(clk_div) | TPM_SC_TOIE_MASK;
}
