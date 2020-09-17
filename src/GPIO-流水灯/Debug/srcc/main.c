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
    gpio_init(LIGHT_RED, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(LIGHT_GREEN, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(LIGHT_BLUE, GPIO_OUTPUT, LIGHT_OFF);

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
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_ON);
            flag = 2;
            break;
        case 2:
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_ON);
            flag = 3;
            break;

        case 3:
            gpio_set(LIGHT_RED, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_BLUE, LIGHT_ON);
            flag = 1;
            break;
        default:
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_OFF);
            flag = 1;
            printf("default");
            break;
        }

    } //for(;;)��β
}