//====================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�20170801-20200502
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//====================================================================

#define GLOBLE_VAR
#include "includes.h"      //������ͷ�ļ�

//----------------------------------------------------------------------
//����ʹ�õ����ڲ�����
//main.cʹ�õ��ڲ�����������
//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //��1��======�������֣���ͷ��==========================================
    //��1.1������main����ʹ�õľֲ�����
    uint32_t mMainLoopCount;  //��ѭ��ʹ�õļ�¼��ѭ����������
    uint8_t  mFlag;            //��ѭ��ʹ�õ���ʱ����

    //��1.2�������䡿�����ж�
    DISABLE_INTERRUPTS;
    wdog_stop();

    //��1.3����������ʹ�õľֲ���������ֵ
    mMainLoopCount = 0;
    mFlag = 3;

    //��1.4����ȫ�ֱ�������ֵ

    //��1.5���û�����ģ���ʼ��
    light_init(LIGHT_RED, LIGHT_OFF);    //��Ƴ�ʼ��
    gpio_init(s0, GPIO_INPUT, 1);
    gpio_init(s1, GPIO_INPUT, 1);

    gpio_pull(s0, 0);
    gpio_pull(s1, 0);


    //��1.7�������䡿�����ж�
    ENABLE_INTERRUPTS;

    uint32_t frequency = 5000000;
    uint32_t number = 1;

    //��2��======��ѭ�����֣���ͷ��========================================
    for (;;)     //for(;;)����ͷ��
    {
        // if (mMainLoopCount++ < 5000000) continue;
        // mMainLoopCount = 0;

        mFlag = gpio_get(s0) + gpio_get(s1);

        printf("\nFlag: %d \n", mFlag);

        switch (mFlag)
        {
        case 0:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 6);
            printf("\n����һ ");
            break;
        case 1:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 4);
            printf("\n���϶� ");
            break;
        case 2:
            light_set(LIGHT_RED, LIGHT_ON);
            light_delay(50000000);
            light_flash(LIGHT_RED, frequency, number * 6);
            break;
        default:
            light_set(LIGHT_RED, LIGHT_OFF);
            printf("\n�м�״̬");
            break;
        }
    }     //for(;;)��β
    //��2��======��ѭ�����֣���β��=====================================
}

//======����Ϊ���������õ��Ӻ�����Ŵ�===================================

//======================================================================
/*
֪ʶҪ�أ�
��1��main.c��һ��ģ�壬���ļ����д�������漰�����Ӳ���ͻ�����ͨ�����ù���
ʵ�ֶ�Ӳ���ĸ�Ԥ��
��2�����ļ��б��С����䡿�ĵط�Ϊϵͳ��������������������Ŀ�޹أ�����ɾ����
��3�����ļ��жԺ�GLOBLE_VAR�����˶��壬�����ڰ���"includes.h"ͷ�ļ�ʱ���ᶨ
��ȫ�ֱ������������ļ��а���"includes.h"ͷ�ļ�ʱ��
����ʱ���Զ�����extern
*/