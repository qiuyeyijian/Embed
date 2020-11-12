//====================================================================
//�ļ����ƣ�main.c��Ӧ�ù�����������
//����ṩ��SD-Arm��sumcu.suda.edu.cn��
//�汾���£�20170801-20200502
//�����������������̵�<01_Doc>�ļ�����Readme.txt�ļ�
//====================================================================

#define GLOBLE_VAR
#include "includes.h" //������ͷ�ļ�

//----------------------------------------------------------------------
//����ʹ�õ����ڲ�����
//main.cʹ�õ��ڲ�����������

//----------------------------------------------------------------------
//��������һ������¿�����Ϊ����Ӵ˿�ʼ���У�ʵ�������������̼���壩
int main(void)
{
    //�����䡿�����ж�
    DISABLE_INTERRUPTS;
    wdog_stop();

    //1. ģ���ʼ��
    light_init(LIGHT_RED, LIGHT_OFF);
    light_init(LIGHT_GREEN, LIGHT_OFF);
    light_init(LIGHT_BLUE, LIGHT_OFF);

    //�����䡿�����ж�
    ENABLE_INTERRUPTS;

    uint32_t loop_count = 0;
    volatile uint8_t flag = 0;

    for (;;) //for(;;)����ͷ��
    {
        if (loop_count++ < 10000000)
            continue;

        loop_count = 0;

        switch (flag)
        {
        case 1:

            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_ON);
            flag = 2;
            break;
        case 2:
            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_ON);
            flag = 3;
            break;

        case 3:
            light_control(LIGHT_RED, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_BLUE, LIGHT_ON);
            flag = 1;
            break;
        default:
            light_control(LIGHT_BLUE, LIGHT_OFF);
            light_control(LIGHT_GREEN, LIGHT_OFF);
            light_control(LIGHT_RED, LIGHT_OFF);
            flag = 1;
            printf("default");
            break;
        }

    } //for(;;)��β
}