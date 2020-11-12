//==========================================================================
//�ļ����ƣ�systick.c
//���ܸ�Ҫ��systick ����Դ�ļ�
//��Ȩ���У����ݴ�ѧǶ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2017-11-08   V1.0
//==========================================================================

#include "systick.h"

//===========================================================================
//�������ƣ�systick_init
//�������أ���
//����˵����int_ms:�жϵ�ʱ����(1~5000ms)
//���ܸ�Ҫ����ʼ��SysTickģ�飬�����жϵ�ʱ����
//��Ҫ˵����ʹ���ں�ʱ�ӣ���Ƶ��SYSTEM_CLK_KHZ��common.h�к궨��
//===========================================================================
void systick_init( uint8_t int_ms)
{
    SysTick->CTRL = 0;   //����ǰ�ȹر�systick��CTRL���Ƽ�״̬�Ĵ���
    SysTick->VAL  = 0;   //�����������VAL ������
    if((int_ms<1)||(int_ms>5592))
    {
       int_ms = 10;
    }
    //SysTick->LOAD = MCU_SYSTEM_CLK_KHZ*int_ms/16;
    SysTick->LOAD = 48000*int_ms/16;
    //�趨 SysTick���ȼ�Ϊ3(SHPR3�Ĵ���������ֽ�=0xC0)
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    //����ʱ��Դ,�����ж�,ʹ�ܸ�ģ��,��ʼ����
    SysTick->CTRL |= ( SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk );
}


