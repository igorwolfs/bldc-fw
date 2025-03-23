#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "inverter_control.h"

#define MCONTROL_STABLE_CYCLES	10 	// Amount of cycles checked for stability
#define MCONTROL_STABLE_CHECK	10 	// Periodic stability check on startup
#define MCONTROL_TIM_PRESCALER	128	
#define MCONTROL_NP				4	
#define MCONTROL_N_STEPS		6	
#define MCONTROL_ADC_STEPS		8 	// ADC steps
#define MCONTORL_TIMER_K 		930232.56 // [60 * TIMER_FREQ / (N_STEPS * NP * PRE)]

typedef struct phase_read {
	union {
		struct {
			float emf;
			float current;
		};
		float buffer[2];
	};
} phase_read_t;

enum control_type
{
	SENSORLESS,
	FOC
};

typedef struct motor_control {
	enum control_type control_type;
	inverter_t *inv;
	phase_read_t *phase_data[INVERTER_N_PHASES];// 3 phases with latest data read
	uint32_t ecycle_count; // Keeps track of amount of control cycles performed (6 steps = 1 cycle).
	int adc_step;
	bool adc_trigger;	// Triggered by TIM1 if ADC measurement required.
	float *	arr[MCONTROL_STABLE_CYCLES];
} motor_control_t;

void main_control(motor_control_t *cmotor);

/**
 *  @brief: Function performing startup by increasing the frequency from flow to fhigh
 */
void mcontrol_linear_startup(motor_control_t *cmotor, float flow, float fhigh);

/**
 * @brief: Function checking back-emf stability
 * @note: It should rely on samples taken between 2 points in the EMF.
 */
void mcontrol_stable_check(motor_control_t *cmotor);

/**
 * @brief: Function to be called in an endless loop keeping speed constant
 * @note:  Measure the ADC here
 */
void mcontrol_sensorless(motor_control_t *cmotor);

/**
 * @brief: Function calculating the pathway to a new speed and setting the new speed.
 * @note: TIM8 used for inverter switching: Initial period 65535, initial rpm: 14. 
 *		  TIM1 used for adc measurement 
 */
int mcontrol_speed_set(motor_control_t *cmotor, float rpm);

#endif