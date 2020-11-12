#include "suart.h"

// �ڲ���������
void suart_get_port_pin(uint16_t port_pin, uint8_t* port, uint8_t* pin);

// ģ�⴮�ڳ�ʼ��
void suart_init(uint16_t port_pin, uint8_t dir) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    //�趨��Ӧ�˿ڵ���Ӧ���Ź�port��ΪGPIO���������ſ��ƼĴ�����MUX=0b001��
    PORT_PCR(port, pin) &= ~PORT_PCR_MUX_MASK;//��D10-D8=000
    PORT_PCR(port, pin) |= PORT_PCR_MUX(1);//��D10-D8=001

    // ���ݴ������dir, ��������Ϊ�����������
    if (dir == 1) {
        // ���ݷ���Ĵ�����Ӧλ��Ϊ1����ʾ���
        BSET(pin, SUART_PDDR(port));

        // ��������Ĵ�����Ӧλ��Ϊ1(û������ʱ���ָߵ�ƽ)����ʾ�ߵ�ƽ
        BSET(pin, SUART_PDOR(port));
    }
    else {
        // ���ݷ���Ĵ�����Ӧλ��Ϊ0����ʾ����
        BCLR(pin, SUART_PDDR(port));
    }
}

void suart_set(uint8_t port, uint8_t pin, uint8_t status) {
    if (status == 1) {
        BSET(pin, SUART_PDOR(port));
    }
    else {
        BCLR(pin, SUART_PDOR(port));
    }
}


uint8_t suart_get(uint8_t port, uint8_t pin) {
    // һ�㶼�ǻ�ȡ��Ӧ���ŵ�����Ĵ�����ֵ
    return BGET(pin, SUART_PDIR(port));
}

// ���������ʵ�ֱ�������ѭ�����������ɼ��ٴ�����
// ���Ǿ������Է��֣�ѭ��ÿ���жϲ���Ҳռ��һ��ʱ��
// ���ܻ�����һ��������whileѭ�������1us�������Ծ�ֱ�Ӷ�ÿһλ����
// �����suart_receive()����ͬ��
void suart_send(uint16_t port_pin, uint8_t* arr, uint32_t len) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    uint8_t bit[8];

    for (uint32_t count = 0; count < len; count++) {
        // ���ַ�ת�ɶ���������
        for (uint8_t i = 0; i < 8; i++) {
            bit[i] = arr[count] >> i & 1;
        }

        // ��ʼλ������ƽ����һ�����ڱ�ʾ��ʼ��������
        suart_set(port, pin, 0);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��0λ
        suart_set(port, pin, bit[0]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��1λ
        suart_set(port, pin, bit[1]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��2λ
        suart_set(port, pin, bit[2]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��3λ
        suart_set(port, pin, bit[3]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��4λ
        suart_set(port, pin, bit[4]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��5λ
        suart_set(port, pin, bit[5]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��6λ
        suart_set(port, pin, bit[6]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��7λ
        suart_set(port, pin, bit[7]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // ��ֹλ, ����ƽ����
        suart_set(port, pin, 1);
        for (uint16_t i = 0; i < delayCount; i++) {}
    }
}


uint32_t suart_receive(uint16_t port_pin, uint8_t* arr) {

    uint8_t port, pin;
    uint16_t count = 4096;
    uint32_t len = 0;
    arr[len] = 0;
    uint8_t bit[8];

    suart_get_port_pin(port_pin, &port, &pin);

    while (count--) {
        // ��ʼλ
        for (uint16_t i = 0; i < delayCount / 2; i++) {}
        suart_get(port, pin);

        // ��0λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[0] = suart_get(port, pin);

        // ��1λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[1] = suart_get(port, pin);

        // ��2λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[2] = suart_get(port, pin);

        // ��3λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[3] = suart_get(port, pin);

        // ��4λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[4] = suart_get(port, pin);

        // ��5λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[5] = suart_get(port, pin);

        // ��6λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[6] = suart_get(port, pin);

        // ��7λ
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[7] = suart_get(port, pin);

        // ��ֹλ, �������10�Σ�����Ϊ��������ִ��ҲҪռ��һ��ʱ��
        // ����������ֹλ��ʱ����������Щ����
        for (uint16_t i = 0; i < delayCount - 10; i++) {}

        for (uint8_t i = 0; i < 8; i++) {
            arr[len] += (bit[i] << i);
        }
        if (!(arr[len] ^ 255)) {
            PORT_PCR(port, pin) |= PORT_PCR_ISF(1); // ����ж�״̬
            return len;
        }
        count = 4096;
        arr[++len] = 0;

        // ���ĵ���ֹλʣ���ʱ�䣬���������һ���ַ��Ľ���
        while (count--) {
            if (!suart_get(port, pin)) {
                break;
            }
        }
    }
}

// �򿪴��ڽ����ж�, �½��ش���
void suart_enable_receive_interupt(uint16_t port_pin) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    // �����ſ��ƼĴ����ж������ֶΣ���D19-D16����
    PORT_PCR(port, pin) &= ~PORT_PCR_IRQC_MASK;
    // ��D19-D16�ó�1010�� ��ʾ�½��ش���
    PORT_PCR(port, pin) |= PORT_PCR_IRQC(10);

    if (port == 0) {
        NVIC_EnableIRQ(PORTA_IRQn);    //ʹ��A�˿��ж�
    }
    else if (port == 2 || port == 3) {
        NVIC_EnableIRQ(PORTC_PORTD_IRQn);    //ʹ��C��D�˿��ж�
    }
    else {
        printf("Fail to init port interupt������\n");
        printf("Please select around Port A or C or D\n");
    }

}


// �ڲ�������Ŵ�
void suart_get_port_pin(uint16_t port_pin, uint8_t* port, uint8_t* pin)
{
    *port = (port_pin >> 8);
    *pin = port_pin;
}
