#ifndef __INVERTER_CONTROL_H
#define __INVERTER_CONTROL_H
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// Phase U
#define GPIO_PIN_U_NSD   GPIO_PIN_0
#define GPIO_PORT_U_NSD  GPIOB

#define GPIO_PIN_U_SW    GPIO_PIN_7
#define GPIO_PORT_U_SW   GPIOA

// Phase V
#define GPIO_PIN_V_NSD   GPIO_PIN_6
#define GPIO_PORT_V_NSD  GPIOB

#define GPIO_PIN_V_SW    GPIO_PIN_1
#define GPIO_PORT_V_SW   GPIOB

// Phase W
#define GPIO_PIN_W_NSD   GPIO_PIN_4
#define GPIO_PORT_W_NSD  GPIOB

#define GPIO_PIN_W_SW    GPIO_PIN_5
#define GPIO_PORT_W_SW   GPIOB

// TEST
#define SW_TEST_1_HIGH   5
#define SW_TEST_2_HIGH   15
#define SW_TEST_3_HIGH   25
#define SW_TEST_1_LOW    35
#define SW_TEST_2_LOW    45
#define SW_TEST_3_LOW    55

typedef struct phase
{
    uint16_t sw_gpio_pin;
    GPIO_TypeDef* sw_gpio_port;
    uint16_t nsd_gpio_pin;
    GPIO_TypeDef* nsd_gpio_port;
} phase_t;

void phase_set(phase_t* phase, bool high);
void phase_unset(phase_t* phase);

// *** FUNCTIONS ***
void regular_switching_cycle(void);
void test_switching_cycle(void);

#endif /* __INVERTER_CONTROL_H */