#include "inverter_control.h"
#include <stdio.h>

// *************** PHASES ********************

void inverter_phase_set(inverter_t *inv, int phase, enum phase_state state)
{
    inv->phases[phase]->state = state;
    switch (state)
    {
        case (PHASE_LOW):
        case (PHASE_HIGH):
            HAL_GPIO_WritePin(inv->phases[phase]->sw_gpio_port, inv->phases[phase]->sw_gpio_pin, (int)state);
            HAL_GPIO_WritePin(inv->phases[phase]->nsd_gpio_port, inv->phases[phase]->nsd_gpio_pin, GPIO_PIN_SET);
            break;
        case (PHASE_OFF):
           HAL_GPIO_WritePin(inv->phases[phase]->nsd_gpio_port, inv->phases[phase]->nsd_gpio_pin, GPIO_PIN_RESET);
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


void inverter_switch_regular(inverter_t *inv)
{
    inv->state %= 6;
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
    inv->state++;
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
}

/**
 * - Internal clock: 48 MHz
 * - No prescaler
 * - Counts till 2**16, so triggers an interrupt 1.365 ms after initialization
 */
