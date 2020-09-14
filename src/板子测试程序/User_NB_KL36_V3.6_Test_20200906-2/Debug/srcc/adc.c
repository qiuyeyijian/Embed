//=====================================================================
//�ļ����ƣ�adc.c
//���ܸ�Ҫ��adc�ײ���������Դ�ļ�
//������λ�����ݴ�ѧǶ��ʽϵͳ���������о���(sumcu.suda.edu.cn)
//��    ����  2019-05-09  V2.0
//����оƬ��KL25��KL26��KL36
//=====================================================================

#include "adc.h"

//�ڲ���������
void adc_cal(void);
void adc_init_inner( uint8_t chnGroup,uint8_t diff,uint8_t accurary,uint8_t HDAve);
uint16_t adc_read_inner(uint8_t channel);


//*********************************�ⲿ���ú���***********************************
//============================================================================
//���ܸ�Ҫ����ʼ��һ��ADͨ����
//����˵����A_SENSOR_No������ģ����������ͨ�������ò����ɲμ�gec.h�ļ�
//       accurary���������ȣ���ֿ�ѡ9-13-11-16�����˿�ѡ8-12-10-16��Խ�߾���Խ��
//============================================================================
void adc_init(uint16_t A_SENSOR_No,uint8_t accurary)
{
	uint8_t chnGroup,diff,acc;
	chnGroup = (uint8_t)(A_SENSOR_No>>8);
	diff = 0;
	acc = accurary;
	if(A_SENSOR_No ==  (0|0<<8))  //MUXSEL_A��Aͨ����
		diff = 1;  //���ѡ��=1�����
	    //MUXSEL_B��Bͨ����
	else if(A_SENSOR_No == (1|1<<8))
		diff = 1;//���ѡ��=1�����
	adc_init_inner(chnGroup,diff,acc,SAMPLE32);
}

//============================================================================
//���ܸ�Ҫ������һ��ͨ����һ��A/Dת��
//����˵����A_SENSOR_No������ģ����������ͨ�������ò����ɲμ�gec.h�ļ�
//============================================================================
uint16_t adc_read(uint16_t A_SENSOR_No)
{
	uint8_t channel;
	channel = (uint8_t)A_SENSOR_No;
	return adc_read_inner(channel);
}

//*********************************�ڲ����ú���***********************************
//============================================================================
//���ܸ�Ҫ����ʼ��һ��ADͨ����
//����˵����chnGroup��ͨ���飻�к곣����MUXSEL_A��Aͨ������MUXSEL_B��Bͨ����
//       diff�����ѡ��=1����֣�=0�����ˣ�Ҳ��ʹ�ú곣��AD_DIFF/AD_SINGLE
//       accurary���������ȣ���ֿ�ѡ9-13-11-16�����˿�ѡ8-12-10-16
//       HDAve��Ӳ���˲��������Ӻ궨����ѡ��SAMPLE4/SAMPLE8/SAMPLE16/SAMPLE32
//============================================================================
void adc_init_inner( uint8_t chnGroup,uint8_t diff,uint8_t accurary,uint8_t HDAve)
{
    uint8_t ADCCfg1;
    //    uint8_t ADCCfg2=0;
    //1.��ADC0ģ��ʱ��
    SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
    //2.����CFG1�Ĵ���:�������ģ�����ʱ��4��Ƶ������ʱ��/2��������ʱ��
    //2.1 ���ݲ�������ȷ��ADC_CFG1_MODEλ
    switch(accurary)
    {
    case 8:case 9:
        ADCCfg1 = ADC_CFG1_MODE(0);
        break;
    case 12:case 13:
        ADCCfg1 = ADC_CFG1_MODE(1);
        break;
    case 10:case 11:
        ADCCfg1 = ADC_CFG1_MODE(2);
        break;
    default:
        ADCCfg1 = ADC_CFG1_MODE(3);
        break;
    }
    //2.2 ������������ֵ���������ģ�����ʱ��4��Ƶ������ʱ��/2��������ʱ�䣩
    ADCCfg1 |=  (ADC_CFG1_ADIV(2) | ADC_CFG1_ADICLK(1) | ADC_CFG1_ADLSMP_MASK);
    //2.3 ��������
    ADC0_CFG1 = ADCCfg1;
    //3.����ͨ���飬����CFG2�Ĵ���
    //3.1����CFG2�Ĵ���
    ADC0_CFG2&=~(ADC_CFG2_ADACKEN_MASK     //�첽ʱ�������ֹ
            + ADC_CFG2_ADHSC_MASK       //��ͨת��
            + ADC_CFG2_ADLSTS_MASK);    //Ĭ�������ʱ��
    //3.2 ѡ��bͨ������aͨ��
    (chnGroup==1)?(ADC0_CFG2 |=(ADC_CFG2_MUXSEL(1))):(ADC0_CFG2 &=~(ADC_CFG2_MUXSEL(1)));
    //4.����ADC0_SC2������������ȽϹ��ܽ��ã�DMA���ã�Ĭ���ⲿ�ο���ѹ VREFH/VREFL
    ADC0_SC2 = 0;
    //5.ADC0_SC3�Ĵ���Ӳ����ֵʹ�ܣ�����Ӳ���˲�����
    ADC0_SC3 |= (ADC_SC3_ADCO_MASK | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS((uint8_t)HDAve));

    //ѡ����������ǵ�������
    if (1 == diff)     //ѡ��������
    {
        ADC0_SC1A |= (ADC_SC1_DIFF_MASK);
        adc_cal();    //����������У��
    }
    else                          //ѡ�񵥶�����
    {
        ADC0_SC1A &= ~(ADC_SC1_DIFF_MASK );
    }
    //����ADCģ���ж�
    ADC0_SC1A &= ~(ADC_SC1_AIEN_MASK);
}

//============================================================================
//���ܸ�Ҫ������һ��ͨ����һ��A/Dת��
//����˵����channel����MKL25Z128VLK4оƬADCͨ�������
//============================================================================
uint16_t adc_read_inner(uint8_t channel)
{
    uint16_t ADCResult = 0;
    //����SC1A�Ĵ���ͨ����
    ADC0_SC1A = ADC_SC1_ADCH(channel);        
    //�ȴ�ת�����
    while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));
    //��ȡת�����
    ADCResult = (uint16_t)ADC0_RA;
    //��ADCת����ɱ�־
    ADC0_SC1A &= ~ADC_SC1_COCO_MASK;
    //���ض�ȡ���
    return ADCResult;
}

//-------�ڲ�����-------------------------------------------------------------
//============================================================================
//���ܸ�Ҫ��adcģ��У�����ܺ���       
//˵������У��֮ǰ������ȷ����ADCʱ�ӡ�����ʱ�䡢ģʽ��Ӳ���˲�32�Σ�
//    ���KL25оƬ�ֲ�28.4.6
//============================================================================
void adc_cal(void)
{
    uint8_t cal_var;

    ADC0_SC2 &=  ~ADC_SC2_ADTRG_MASK ; // ʹ���������
    ADC0_SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); //����ת��
    ADC0_SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3) );  //Ӳ��ƽ���˲�32��
    ADC0_SC3 |= ADC_SC3_CAL_MASK ;      //��ʼУ��
    while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // �ȴ�ת�����

    if (ADC0_SC3& ADC_SC3_CALF_MASK) goto adc_cal_exit; // У��ʧ��
    // У����ȷ������ִ��
    // ������������У��
    cal_var = 0x00;
    cal_var =  ADC0_CLP0;
    cal_var += ADC0_CLP1;
    cal_var += ADC0_CLP2;
    cal_var += ADC0_CLP3;
    cal_var += ADC0_CLP4;
    cal_var += ADC0_CLPS;

    cal_var = cal_var/2;
    cal_var |= 0x8000;   //Set MSB
    ADC0_PG = ADC_PG_PG(cal_var);

    // ���㸺������У��
    cal_var = 0x00;
    cal_var =  ADC0_CLM0;
    cal_var += ADC0_CLM1;
    cal_var += ADC0_CLM2;
    cal_var += ADC0_CLM3;
    cal_var += ADC0_CLM4;
    cal_var += ADC0_CLMS;

    cal_var = cal_var/2;
    cal_var |= 0x8000; // Set MSB
    ADC0_MG = ADC_MG_MG(cal_var);
    ADC0_SC3 &= ~ADC_SC3_CAL_MASK ; //��CAL
adc_cal_exit:
    __asm("NOP");
}


//============================================================================
//���ܸ�Ҫ����������mcu�¶�ADֵת��Ϊʵ���¶�
//����˵����mcu_temp_AD��ͨ��adc_read�����õ���ADֵ
//�������أ�ʵ���¶�ֵ
//============================================================================
float TempTrans(uint16_t mcu_temp_AD)
{
	float mcu_temp_result;
	mcu_temp_result=(float)(25.0-(mcu_temp_AD*0.05-719)/1.715);
	return mcu_temp_result;
}