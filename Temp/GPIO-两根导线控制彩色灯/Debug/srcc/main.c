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
    gpio_init(SWITCH_0, GPIO_INPUT, LIGHT_OFF);
    gpio_init(SWITCH_1, GPIO_INPUT, LIGHT_OFF);

    //2. ��������
    gpio_pull(SWITCH_0, PULL_DOWN);
    gpio_pull(SWITCH_1, PULL_DOWN);

    //

    //�����䡿�����ж�
    ENABLE_INTERRUPTS;

    uint32_t mMainLoopCount; //��ѭ��ʹ�õļ�¼��ѭ����������

    for (;;) //for(;;)����ͷ��
    {
        // ��ѭ������+1�����ж��Ƿ�С���ض�����
        mMainLoopCount++; //+1
        if (mMainLoopCount <= 13113354)
            continue;       //���С���ض�����������ѭ��
        mMainLoopCount = 0; //����ѭ������

        // ȡ��Ӧ���ŵ�����״̬
        volatile uint8_t switch_0_status = gpio_get(SWITCH_0);
        volatile uint8_t switch_1_status = gpio_get(SWITCH_1);

        // volatile uint8_t flag = (portC_pin_1_state << 1) | portC_pin_1_state;
        volatile uint8_t flag = (switch_0_status << 1) | switch_1_status;
        printf("\nflag: %d", flag);

        switch (flag)
        {
        case 1:
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_ON); //�̵���

            break;
        case 2:
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_ON);
            break;

        case 3:
            gpio_set(LIGHT_RED, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_BLUE, LIGHT_ON);
            break;
        default:
            gpio_set(LIGHT_BLUE, LIGHT_OFF);
            gpio_set(LIGHT_GREEN, LIGHT_OFF);
            gpio_set(LIGHT_RED, LIGHT_OFF);
            printf("default");
            break;
        }

    } //for(;;)��β
}