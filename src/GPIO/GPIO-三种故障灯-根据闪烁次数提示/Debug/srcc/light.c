//===================================================================
//文件名称：light.c
//功能概要：故障灯构件源文件
//设计者：1813021018-沈晔-计181
//更新记录：2020/9/27
//===================================================================

#include "light.h"

void light_delay(uint32_t delay_time) {
    for (;;) {
        if (delay_time-- > 0) continue;
        break;
    }
}

//(函数头注释见头文件)
void light_init(uint16_t port_pin, uint8_t state)
{
    gpio_init(port_pin, GPIO_OUTPUT, state);
}

//(函数头注释见头文件)
void light_set(uint16_t port_pin, uint8_t state)
{
    gpio_set(port_pin, state);
}

//(函数头注释见头文件)
void light_flash(uint16_t port_pin, uint32_t frequency, uint8_t number)
{
    for (uint8_t i = 0; i < number; i++) {
        uint32_t temp1 = frequency;
        uint32_t temp2 = frequency;

        printf("开始闪了: %d！\n", i);
        gpio_set(port_pin, LIGHT_OFF);

        for (;;) {
            if (temp1-- > 0) continue;
            break;
        }

        gpio_set(port_pin, LIGHT_ON);

        for (;;) {
            if (temp2-- > 0) continue;
            break;
        }

    }
}

//(函数头注释见头文件)
void light_reverse(uint16_t port_pin)
{
    gpio_reverse(port_pin);
}