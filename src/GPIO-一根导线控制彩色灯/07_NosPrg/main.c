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
    gpio_init(LIGHT_BLUE, GPIO_OUTPUT, LIGHT_OFF);
    gpio_init(SWITCH_0, GPIO_INPUT, LIGHT_OFF);

    //2. ������������
    gpio_pull(SWITCH_0, PULL_DOWN);

    //�����䡿�����ж�
    ENABLE_INTERRUPTS;

    for (;;) //for(;;)����ͷ��
    {
        // ȡ��Ӧ���ŵ�����״̬
        volatile uint8_t portC_pin_0_state = gpio_get(SWITCH_0);

        printf("input, %d", portC_pin_0_state);

        if (portC_pin_0_state)
        {
            gpio_set(LIGHT_BLUE, LIGHT_ON);
            printf("\n ������");
        }
        else
        {
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            printf("\n ������");
        }

    } //for(;;)��β
}