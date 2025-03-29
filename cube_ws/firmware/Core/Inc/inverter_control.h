#ifndef __INVERTER_CONTROL_H
#define __INVERTER_CONTROL_H
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// *****************************
// ********* SETTINGS **********
// *****************************

#define INVERTER_N_PHASES 3
// *****************************

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

// *** PHASES ***
enum phase_state {
    PHASE_LOW = 0,
    PHASE_HIGH = 1,
    PHASE_OFF
};

typedef struct phase_conf
{
    uint16_t sw_gpio_pin;
    GPIO_TypeDef* sw_gpio_port;
    void (*nsd_pwm_start)(void);           //< pwm_start function
    void (*nsd_pwm_stop)(void);            //< pwm_stop function
    void (*nsd_pwm_d)(int duty_cycle);     //< pwm_duty cycle-set function
    enum phase_state state;
} phase_conf_t;

enum phase {
    PHASE_U=0,
    PHASE_V=1,
    PHASE_W=2
};

typedef struct inverter
{
    phase_conf_t *phases[INVERTER_N_PHASES];
    int state;
} inverter_t;

// *** INVERTER ***

int inverter_init(inverter_t *inv, phase_conf_t **phase_ptr, int phase_count);
int inverter_align(inverter_t *inv);
void inverter_phase_set(inverter_t *inv, int phase, enum phase_state state);
void inverter_switch_regular(inverter_t *inverter);
enum phase inverter_get_inactive(inverter_t *inv);


// NSD START
void inverter_nsd_pwm_w_start(void);
void inverter_nsd_pwm_v_start(void);
void inverter_nsd_pwm_u_start(void);


// NSD STOP 
void inverter_nsd_pwm_w_stop(void);
void inverter_nsd_pwm_v_stop(void);
void inverter_nsd_pwm_u_stop(void);


// DUTY CYCLE SETTING
void inverter_nsd_pwm_w_d(int duty_cycle);
void inverter_nsd_pwm_v_d(int duty_cycle);
void inverter_nsd_pwm_u_d(int duty_cycle);
/**
 * TODO
 * - Add phases with malloc so arbitrary number of phases can be added
 * - Add possible inverter states in enum
 * - Add different inverter switching sequences in enum as array to be repeated (e.g.: [1, 3, 5, 7, 9] with each number representing a state assigned by default)
 * 
 */

#endif /* __INVERTER_CONTROL_H */