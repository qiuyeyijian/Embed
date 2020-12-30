#include "suart.h"

// 内部函数声明
void suart_get_port_pin(uint16_t port_pin, uint8_t* port, uint8_t* pin);

// 模拟串口初始化
void suart_init(uint16_t port_pin, uint8_t dir) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    //设定相应端口的相应引脚功port能为GPIO（即令引脚控制寄存器的MUX=0b001）
    PORT_PCR(port, pin) &= ~PORT_PCR_MUX_MASK;//置D10-D8=000
    PORT_PCR(port, pin) |= PORT_PCR_MUX(1);//置D10-D8=001

    // 根据带入参数dir, 决定引脚为输出还是输入
    if (dir == 1) {
        // 数据方向寄存器对应位设为1，表示输出
        BSET(pin, SUART_PDDR(port));

        // 数据输出寄存器对应位设为1(没有数据时保持高电平)，表示高电平
        BSET(pin, SUART_PDOR(port));
    }
    else {
        // 数据方向寄存器对应位设为0，表示输入
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
    // 一般都是获取相应引脚的输入寄存器的值
    return BGET(pin, SUART_PDIR(port));
}

// 这个函数的实现本可以用循环来操作，可减少代码量
// 但是经过测试发现，循环每次判断操作也占用一定时间
// 可能会引起一定的误差（用while循环，大概1us），所以就直接对每一位操作
// 下面的suart_receive()函数同理
void suart_send(uint16_t port_pin, uint8_t* arr, uint32_t len) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    uint8_t bit[8];

    for (uint32_t count = 0; count < len; count++) {
        // 将字符转成二进制数组
        for (uint8_t i = 0; i < 8; i++) {
            bit[i] = arr[count] >> i & 1;
        }

        // 起始位，将电平拉低一个周期表示开始发送数据
        suart_set(port, pin, 0);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第0位
        suart_set(port, pin, bit[0]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第1位
        suart_set(port, pin, bit[1]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第2位
        suart_set(port, pin, bit[2]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第3位
        suart_set(port, pin, bit[3]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第4位
        suart_set(port, pin, bit[4]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第5位
        suart_set(port, pin, bit[5]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第6位
        suart_set(port, pin, bit[6]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 第7位
        suart_set(port, pin, bit[7]);
        for (uint16_t i = 0; i < delayCount; i++) {}

        // 终止位, 将电平拉高
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
        // 起始位
        for (uint16_t i = 0; i < delayCount / 2; i++) {}
        suart_get(port, pin);

        // 第0位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[0] = suart_get(port, pin);

        // 第1位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[1] = suart_get(port, pin);

        // 第2位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[2] = suart_get(port, pin);

        // 第3位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[3] = suart_get(port, pin);

        // 第4位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[4] = suart_get(port, pin);

        // 第5位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[5] = suart_get(port, pin);

        // 第6位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[6] = suart_get(port, pin);

        // 第7位
        for (uint16_t i = 0; i < delayCount; i++) {}
        bit[7] = suart_get(port, pin);

        // 终止位, 这里减少10次，是因为下面的语句执行也要占用一定时间
        // 所以利用终止位的时间来进行这些运算
        for (uint16_t i = 0; i < delayCount - 10; i++) {}

        for (uint8_t i = 0; i < 8; i++) {
            arr[len] += (bit[i] << i);
        }
        if (!(arr[len] ^ 255)) {
            arr[len] = '\0';
            PORT_PCR(port, pin) |= PORT_PCR_ISF(1); // 清除中断状态
            return len;
        }
        count = 4096;
        arr[++len] = 0;

        // 消耗掉终止位剩余的时间，方便进行下一个字符的接收
        while (count--) {
            if (!suart_get(port, pin)) {
                break;
            }
        }
    }
}

// 打开串口接收中断, 下降沿触发
void suart_enable_receive_interupt(uint16_t port_pin) {
    uint8_t port, pin;
    suart_get_port_pin(port_pin, &port, &pin);

    // 清引脚控制寄存器中断配置字段，将D19-D16置零
    PORT_PCR(port, pin) &= ~PORT_PCR_IRQC_MASK;
    // 将D19-D16置成1010， 表示下降沿触发
    PORT_PCR(port, pin) |= PORT_PCR_IRQC(10);

    if (port == 0) {
        NVIC_EnableIRQ(PORTA_IRQn);    //使能A端口中断
    }
    else if (port == 2 || port == 3) {
        NVIC_EnableIRQ(PORTC_PORTD_IRQn);    //使能C、D端口中断
    }
    else {
        printf("Fail to init port interupt！！！\n");
        printf("Please select around Port A or C or D\n");
    }

}


// 内部函数存放处
void suart_get_port_pin(uint16_t port_pin, uint8_t* port, uint8_t* pin)
{
    *port = (port_pin >> 8);
    *pin = port_pin;
}
