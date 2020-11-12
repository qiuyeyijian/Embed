#include "light.h"

void light_init(uint16_t port_pin, uint8_t state)
{
    gpio_init(port_pin, GPIO_OUTPUT, state);
}

void light_control(uint16_t port_pin, uint8_t state)
{
    gpio_set(port_pin, state);
}

void light_change(uint16_t port_pin)
{
    gpio_reverse(port_pin);
}