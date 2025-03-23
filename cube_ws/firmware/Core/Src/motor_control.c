#include "motor_control.h"
#include <math.h>
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
extern TIM_HandleTypeDef htim1;

int main_control(motor_control_t *cmotor)
{
	// IN RPM
	float starting_speed = 14.0; // rpm
	float target_speed = 1200; // rpm
	int n_steps = 100; // (1200.0-14.0) / 100
	float stepsize = (target_speed-starting_speed) / (float) (n_steps);

	// STARTUP LOOP
	int step = 0;
	while (1)
	{
		printf(".");
		float set_speed = ((float)step) * stepsize + starting_speed;
		mcontrol_speed_set(cmotor, set_speed);
		// Step should be increased inside the interrupt
		if (cmotor->adc_trigger)
		{
			// Get non-energized phase, measure back-emf
			enum phase ph_inactive = inverter_get_inactive(cmotor->inv);
			mcontrol_read_phase(cmotor, ph_inactive);
		}

		if (cmotor->ecycle_count == MCONTROL_STABLE_CHECK)
		{
			cmotor->ecycle_count = 0;
			step++;
			// Check the stability of the back-emf after MCONTROL_STABLE_CYCLES cycles
			if (!mcontrol_stable_check(cmotor))
			{
				break; // Proceed to regular control loop
			}
		}
	}

	// Control loop
	while (1)
	{
		/**
		 * Adapt period of next timeout depending on the adc-measurement instead of the other way around
		 */
		
	}
	return 0;
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
	cmotor->control_type = SENSORLESS;
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
		if (cmotor->hist_arr[2*i] * cmotor->hist_arr[2*i+1] > 0)
		{
			return -1;
		}
	}
	// Zero crossing occurs for all MCONTROL_STABLE_CHECK-cycles
	return 0;
}


int mcontrol_speed_set(motor_control_t *cmotor, float rpm) {

	static int timer_init = false;
	if (!timer_init) {
		HAL_TIM_Base_Start_IT(&htim8);
		timer_init = true;
	}
	// *** MOTOR CONTORL SPEED SETTING
	// mech_speed = (k_factor/(timer_period)) / 8 
	float ptimer_cmotor_f = MCONTORL_TIMER_K / rpm;
	if ((ptimer_cmotor_f > 65535) || (ptimer_cmotor_f < 1))
	{
		printf("Error, timer periods too large / too small");
		return -1;
	}
	uint16_t ptimer_ui16 = (uint16_t) roundf(ptimer_cmotor_f);
	__HAL_TIM_SET_AUTORELOAD(&htim8, ptimer_ui16); // Set appropriate period
	return -1;
}

/**
** TIMERS VS SLEEP / MAIN LOOP
Change timer period while running according to speed
 */