#include "inverter_control.h"


void phase_set(phase_t* phase, bool high)
{
    HAL_GPIO_WritePin(phase->sw_gpio_port, phase->sw_gpio_pin, (int)high);
    HAL_GPIO_WritePin(phase->nsd_gpio_port, phase->nsd_gpio_pin, GPIO_PIN_SET);
}

void phase_unset(phase_t* phase)
{
    HAL_GPIO_WritePin(phase->nsd_gpio_port, phase->nsd_gpio_pin, GPIO_PIN_RESET);
}


void regular_switching_cycle(void)
{
    printf(">");
    static period_counter = 0;
    period_counter %= 6;
    switch(period_counter) 
    {
    case (0):
        // SW U OFF
        HAL_GPIO_WritePin(GPIO_PORT_U_SW, GPIO_PIN_U_SW, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_U_NSD, GPIO_PIN_U_NSD, GPIO_PIN_RESET);//GPIO_PIN_SET);
        // SW W OFF
        HAL_GPIO_WritePin(GPIO_PORT_W_SW, GPIO_PIN_W_SW, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_PORT_W_NSD, GPIO_PIN_W_NSD, GPIO_PIN_RESET);
        break;
    case (1):
        HAL_GPIO_WritePin(GPIO_PORT_W_NSD, GPIO_PIN_W_NSD, GPIO_PIN_RESET);//GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_V_NSD, GPIO_PIN_V_NSD, GPIO_PIN_RESET);
        break;
    case (2):
        HAL_GPIO_WritePin(GPIO_PORT_V_SW, GPIO_PIN_V_SW, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_V_NSD, GPIO_PIN_V_NSD, GPIO_PIN_RESET); // GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_U_SW, GPIO_PIN_U_SW, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_PORT_U_NSD, GPIO_PIN_U_NSD, GPIO_PIN_RESET);
        break;
    case (3):
        HAL_GPIO_WritePin(GPIO_PORT_U_NSD, GPIO_PIN_U_NSD, GPIO_PIN_RESET); // GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_W_NSD, GPIO_PIN_W_NSD, GPIO_PIN_RESET);
        break;
    case (4):
        HAL_GPIO_WritePin(GPIO_PORT_W_SW, GPIO_PIN_W_SW, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_W_NSD, GPIO_PIN_W_NSD, GPIO_PIN_RESET); // GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_V_SW, GPIO_PIN_V_SW, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_PORT_V_NSD, GPIO_PIN_V_NSD, GPIO_PIN_RESET);
        break;
    case (5):
        HAL_GPIO_WritePin(GPIO_PORT_V_NSD, GPIO_PIN_V_NSD, GPIO_PIN_RESET);//GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_PORT_U_NSD, GPIO_PIN_U_NSD, GPIO_PIN_RESET);
        break;
    }
    period_counter++;
}


/**
 * - Internal clock: 48 MHz
 * - No prescaler
 * - Counts till 2**16, so triggers an interrupt 1.365 ms after initialization
 */



void test_switching_cycle(phase_t phase_u, phase_t phase_v, phase_t phase_w)
{
    printf(">");
    static period_counter = 0;
    period_counter %= 60;
    switch (period_counter)
    {
        case (SW_TEST_1_HIGH):
            phase_set(&phase_u, true);
            break;
        case (SW_TEST_2_HIGH):
            phase_set(&phase_v, true);
            break;
        case (SW_TEST_3_HIGH):
            phase_set(&phase_w, true);
            break;
        case (SW_TEST_1_LOW):
            phase_set(&phase_u, false);
            break;
        case (SW_TEST_2_LOW):
            phase_set(&phase_v, false);
            break;
        case (SW_TEST_3_LOW):
            phase_set(&phase_w, false);
            break;
        default:
            phase_unset(&phase_u);
            phase_unset(&phase_v);
            phase_unset(&phase_w);
    }

    period_counter++;
}