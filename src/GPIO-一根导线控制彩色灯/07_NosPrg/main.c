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

    //1. �ҵ��˿���Ӧ�Ļ���ַ�����Բο�KL36�ο��ֲ�
    volatile uint32_t *portA_base = (uint32_t *)0x40049000u;
    volatile uint32_t *portC_base = (uint32_t *)0x4004B000u;

    //2. �ҵ��˿���Ӧ���ſ������ĵ�ַ
    volatile uint32_t *portA_pcr_5 = portA_base + 5;
    volatile uint32_t *portC_pcr_0 = portC_base + 0;

    //3. ���˿ڵ���Ӧ�������ó� GPIO
    *portA_pcr_5 &= ~0x700u; //��MUXλ��
    *portA_pcr_5 |= 0x100u;
    *portC_pcr_0 &= ~0x700u;
    *portC_pcr_0 |= 0x100u;

    //4. �ҵ��˿���ӦGPIO�Ĵ����Ļ���ַ
    volatile uint32_t *gpioA_base = (uint32_t *)0x400FF000u;
    volatile uint32_t *gpioC_base = (uint32_t *)0x400FF080u;

    //6. �ҵ��˿ڵ� GPIO��������Ĵ��� pdor
    volatile uint32_t *portA_pdor = gpioA_base + 0;
    volatile uint32_t *portC_pdor = gpioC_base + 0;

    //7. �ҵ��˿ڵ� GPIO��������Ĵ��� pdir
    volatile uint32_t *portA_pdir = gpioA_base + 4;
    volatile uint32_t *portC_pdir = gpioC_base + 4;

    //8. �ҵ��˿ڵ�GPIO���ݷ���Ĵ�����ַ
    volatile uint32_t *portA_pddr = gpioA_base + 5;
    volatile uint32_t *portC_pddr = gpioC_base + 5;

    //9. ������Ӧ���ŵ��������״̬
    *portA_pddr |= (1 << 5);
    *portC_pddr &= ~(1 << 0);

    //================================================================
    // �ԵƵ�״̬���г�ʼ��
    *portA_pdor |= (1 << 5);

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
        volatile uint8_t portC_pin_0_state = (*portC_pdir >> 0) & 1;

        printf("Hello, %d", portC_pin_0_state);

        if (portC_pin_0_state)
        {
            *portA_pdor &= ~(1 << 5); //��
            printf("\n������");
        }
        else
        {
            *portA_pdor |= (1 << 5);
            printf("\n������");
        }

    } //for(;;)��β
}