#include "inverter_control.h"
#include <stdio.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

// *************** PHASES ********************

void inverter_phase_set(inverter_t *inv, int phase, enum phase_state state)
{
    inv->phases[phase]->state = state;
    switch (state)
    {
        case (PHASE_LOW):
        case (PHASE_HIGH):
            HAL_GPIO_WritePin(inv->phases[phase]->sw_gpio_port, inv->phases[phase]->sw_gpio_pin, (int)state);
            inv->phases[phase]->nsd_pwm_start();
            break;
        case (PHASE_OFF):
            inv->phases[phase]->nsd_pwm_stop();
            break;
    }
}

// *************** INVERTER ********************

int inverter_init(inverter_t *inv, phase_conf_t **phase_ptr, int phase_count)
{
    // u, v, w -> 0, 1, 2
    for (int ph_i=0; ph_i<phase_count; ph_i++)
    {
        if (phase_ptr[ph_i] == NULL)
        {
            return -1;
        }
        inv->phases[ph_i] = phase_ptr[ph_i];
        inverter_phase_set(inv, ph_i, PHASE_OFF);
    }
    return 0;
}


int inverter_align(inverter_t *inv)
{
    inverter_phase_set(inv, 0, PHASE_HIGH);
    inverter_phase_set(inv, 2, PHASE_OFF);
}


void inverter_switch_regular(inverter_t *inv)
{
    switch(inv->state)
    {
        case (0):
            inverter_phase_set(inv, 0, PHASE_HIGH);
            inverter_phase_set(inv, 2, PHASE_OFF);
            break;
        case (1):
            inverter_phase_set(inv, 2, PHASE_LOW);
            inverter_phase_set(inv, 1, PHASE_OFF);
            break;
        case (2):
            inverter_phase_set(inv, 1, PHASE_HIGH);
            inverter_phase_set(inv, 0, PHASE_OFF);
            break;
        case (3):
            inverter_phase_set(inv, 0, PHASE_LOW);
            inverter_phase_set(inv, 2, PHASE_OFF);
            break;
        case (4):
            inverter_phase_set(inv, 2, PHASE_HIGH);
            inverter_phase_set(inv, 1, PHASE_OFF);
            break;
        case (5):
            inverter_phase_set(inv, 1, PHASE_LOW);
            inverter_phase_set(inv, 0, PHASE_OFF);
            break;
    }
    //! WARNING: state must always be valid
    inv->state = (inv->state >= 5) ? 0 : (inv->state + 1);
}

enum phase inverter_get_inactive(inverter_t *inv)
{
    for (int i=0; i<INVERTER_N_PHASES; i++)
    {
        // HAL GPIO Get state -> if 
        if (inv->phases[i]->state == PHASE_OFF)
        {
            return (enum phase)i;
        }
    }
    if (inv->phases[0]->state == PHASE_OFF) {
        return PHASE_U;
    }
    else if (inv->phases[1]->state == PHASE_OFF) {
        return PHASE_V;
    }
    else {
        return PHASE_W;
    }
}

/**
 * - Internal clock: 48 MHz
 * - No prescaler
 * - Counts till 2**16, so triggers an interrupt 1.365 ms after initialization
 */

//! NSD FUNCTIONS
// STOP 
void inverter_nsd_pwm_w_stop(void) {
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}
void inverter_nsd_pwm_v_stop(void) {
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}
void inverter_nsd_pwm_u_stop(void) {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}


// ACTIVATION 
void inverter_nsd_pwm_w_start(void) {
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}
void inverter_nsd_pwm_v_start(void) {
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}
void inverter_nsd_pwm_u_start(void) {
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}


// DUTY CYCLE SETTING
void inverter_nsd_pwm_w_d(int duty_cycle)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)(duty_cycle*100));
}
void inverter_nsd_pwm_v_d(int duty_cycle)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, (uint32_t)(duty_cycle*100));
}
void inverter_nsd_pwm_u_d(int duty_cycle)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)(duty_cycle*100));
}