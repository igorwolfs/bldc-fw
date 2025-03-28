#include "motor_control.h"
#include <math.h>
#include "adc_read.h"
/**
TODO here
Combine the functions from
- inverter_control
- adc_read
In order to
- Measure the currently inactive phase using read_phase_u
- Set the next phase using inverter_control
	- Using the function inverter_switch_regular, being called every x seconds
CODE:
- struct contains the inverter_t struct
- struct contains the step
- struct contains the 
- Depending on this component it should
	- 

 */


extern TIM_HandleTypeDef htim8;

int main_control(motor_control_t *cmotor)
{
	// *****************************
	// WAIT FOR POWER
	// *****************************
	
	// while (1)
	// {
	// 	adc_vref_read();
	// 	float vbat = 0.0;
	// 	adc_read_vbat(&vbat);
	// 	printf("VBAT: %.2f\r\n", vbat);
	// 	if (vbat > MCONTROL_VBAT_MIN) {
	// 		printf("Control start\r\n");
	// 		break;
	// 	}
	// 	HAL_Delay(1000);
	// }

	printf("main_control\r\n");
	// **********************
	// STARTER SETTINGS
	// **********************
	float starting_speed = 30.0; // rpm
	float target_speed = 30.0; // rpm
	int n_steps = 100; // (1200.0-14.0) / 100
	float stepsize = (target_speed-starting_speed) / (float) (n_steps);

	// STARTUP LOOP
	int step_i = 0, step = 0;
	printf("loop start\r\n");
	mcontrol_power_set(cmotor, 30);
	mcontrol_speed_set(cmotor, starting_speed);
	// mcontrol_align(cmotor);

	
	while (1)
	{
		if (step_i >= 10)
		{
			step_i = 0;
			step++;
			//! TEMP FOR TESTING
			if (step >= n_steps)
			{
				step = 0;
			}
			float set_speed = ((float)step) * stepsize + starting_speed;
			printf("speed set: %d RPM", (int) roundf(set_speed));
			mcontrol_speed_set(cmotor, set_speed);
		}
		// Step should be increased inside the interrupt
		if (cmotor->adc_trigger)
		{
			// Get non-energized phase, measure back-emf
			enum phase ph_inactive = inverter_get_inactive(cmotor->inv);
			mcontrol_read_phase(cmotor, ph_inactive);
			printf("[(%d,%d),%.2f]", ph_inactive, cmotor->adc_step, cmotor->phase_data[ph_inactive]->emf);
			if (cmotor->adc_step == (MCONTROL_ADC_STEPS-1))
			{
				printf("\r\n");
			}
			cmotor->adc_trigger = false;
		}
		if (cmotor->ecycle_count >= MCONTROL_STABLE_CHECK)
		{
			printf(".\r\n");
			step_i++;
			cmotor->ecycle_count = 0;
			// Check the stability of the back-emf after MCONTROL_STABLE_CYCLES cycles
			if (!mcontrol_stable_check(cmotor))
			{
				break; // Proceed to regular control loop
			}
			float vbat;
			adc_read_vbat(&vbat);
			// if (vbat < MCONTROL_VBAT_MIN)
			// {
			// 	mcontrol_speed_set(cmotor, 0);
			// 	break;
			// }
		}
	}

	// Control loop
	// while (1)
	// {
		// Adapt period of next timeout depending on the adc-measurement instead of the other way around
		
	// }
	return 0;
}


void mcontrol_power_set(motor_control_t *cmotor, uint8_t duty)
{
	uint8_t duty_limited =  (duty > 100) ? 100 : duty;
	for (int ph_i=0; ph_i<INVERTER_N_PHASES; ph_i++)
	{
		cmotor->inv->phases[ph_i]->nsd_pwm_d(duty);
	}
}

void mcontrol_align(motor_control_t *cmotor)
{
	inverter_align(cmotor->inv);
	HAL_Delay(250);
}

int mcontrol_init(motor_control_t *cmotor, phase_read_t **phase_data_ptr, int phase_count)
{
    for (int ph_i=0; ph_i<phase_count; ph_i++)
    {
        if (phase_data_ptr[ph_i] == NULL)
        {
            return -1;
        }
        cmotor->phase_data[ph_i] = phase_data_ptr[ph_i];
    }
	cmotor->control_type = FOC;
	memset(&cmotor->hist_arr[0], 0, sizeof(cmotor->hist_arr));
	cmotor->ecycle_count = 0;
	cmotor->adc_step = 0;
	cmotor->adc_trigger = false;
	return 0;
}

int mcontrol_stable_check(motor_control_t *cmotor) {
	for (int i=0; i<(MCONTROL_STABLE_CHECK * MCONTROL_N_STEPS); i++)
	{
		// Check whether a zero-crossing occurs
		if (cmotor->hist_arr[2*i] * cmotor->hist_arr[2*i+1] < 0)
		{
			return -1;
		}
	}
	// Zero crossing occurs for all MCONTROL_STABLE_CHECK-cycles
	// return 0;
	return -1;
}


int mcontrol_speed_set(motor_control_t *cmotor, float rpm) {

	static int timer_init = false;
	if (rpm == 0) {
		HAL_TIM_Base_Stop_IT(&htim8);
		timer_init = false;
		return -1;
	}
	else if (!timer_init) {
		HAL_TIM_Base_Start_IT(&htim8);
		timer_init = true;
	}

	// *** MOTOR CONTORL SPEED SETTING
	float ptimer_cmotor_f = MCONTROL_K / rpm;
	if ((ptimer_cmotor_f > 65535.0) || (ptimer_cmotor_f < 1.0))
	{
		printf("Error, timer periods too large / too small %.2f", rpm);
		return -1;
	}
	uint16_t ptimer_ui16 = (uint16_t) roundf(ptimer_cmotor_f);
	__HAL_TIM_SET_AUTORELOAD(&htim8, ptimer_ui16); // Set appropriate period
	// printf("RPM: %d\r\n", (int)roundf(rpm));
	return 0;
}


/**
** TIMERS VS SLEEP / MAIN LOOP
Change timer period while running according to speed
 */