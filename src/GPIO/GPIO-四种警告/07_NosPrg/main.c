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

void light_flash(uint16_t port_pin, uint32_t delay_time);

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

    for (;;) //for(;;)����ͷ��
    {
        // ȡ��Ӧ���ŵ�����״̬
        volatile uint8_t switch_0_status = gpio_get(SWITCH_0);
        volatile uint8_t switch_1_status = gpio_get(SWITCH_1);

        // volatile uint8_t flag = (portC_pin_1_state << 1) | portC_pin_1_state;
        volatile uint8_t flag = (switch_0_status << 1) | switch_1_status;
        printf("\nflag: %d", flag);

        switch (flag)
        {
        case 0:
            light_flash(LIGHT_RED, 0);
            break;
        case 1:
            light_flash(LIGHT_RED, 1000000);
            break;
        case 2:
            light_flash(LIGHT_RED, 5000000);
            break;
        case 3:
            light_flash(LIGHT_RED, 10000000);
            break;
        default:
            light_flash(LIGHT_RED, 0);
            printf("default");
            break;
        }
    } //for(;;)��β
}

void light_flash(uint16_t port_pin, uint32_t delay_time)
{
    for (;;)
    {
        if (delay_time-- > 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    gpio_reverse(port_pin);
}