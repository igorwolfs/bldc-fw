#include "motor_control.h"

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
		float set_speed = ((float)step) * stepsize + starting_speed;
		mcontrol_speed_set(cmotor, set_speed);
		// Step should be increased inside the interrupt
		if (cmotor->ecycle_count == MCONTROL_STABLE_CHECK)
		{
			cmotor->ecycle_count = 0;
			step++;
			// Check the stability of the back-emf after MCONTROL_STABLE_CYCLES cycles
			mcontrol_stable_check(cmotor);
		}
		if (cmotor->adc_trigger)
		{
			
		}
	}
}


void mcontrol_linear_startup(motor_control_t *cmotor, float flow, float fhigh) {

}

void mcontrol_stable_check(motor_control_t *cmotor) {

}


void mcontrol_sensorless(motor_control_t *cmotor) {

}

int mcontrol_speed_set(motor_control_t *cmotor, float rpm) {

	static timer_init = false;
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