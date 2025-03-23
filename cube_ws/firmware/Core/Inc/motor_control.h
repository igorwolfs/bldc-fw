#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "inverter_control.h"
#include <stdint.h>

#define MCONTROL_STABLE_CHECK	10 	// Periods used for BEMF stability check
#define MCONTROL_TIM_PRESCALER	128	
#define MCONTROL_NP				4	
#define MCONTROL_N_STEPS		6	
#define MCONTROL_ADC_STEPS		8 	// ADC steps
#define MCONTROL_ADC_STEP1		2 	// Step for first measurement
#define MCONTROL_ADC_STEP2		6	// Step for second measurement (zero crossing)
#define MCONTROL_TIMER_K 		62500.0 // [60 * TIMER_FREQ / (N_STEPS * NP * PRE)]

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
	// Inverter
	inverter_t *inv;
	// ADC phase data
	phase_read_t *phase_data[INVERTER_N_PHASES];// 3 phases with latest data read
	float hist_arr[MCONTROL_N_STEPS * 2 * MCONTROL_STABLE_CHECK]; // 2 measurements per step
	// Timer data
	uint32_t ecycle_count; // Keeps track of amount of control cycles performed (6 steps = 1 cycle).
	int adc_step;
	bool adc_trigger;	// Triggered by TIM1 if ADC measurement required.
} motor_control_t;


int main_control(motor_control_t *cmotor);

int mcontrol_init(motor_control_t *cmotor, phase_read_t **phase_data_ptr, int phase_count);

/**
 *  @brief: Function performing startup by increasing the frequency from flow to fhigh
 */
void mcontrol_linear_startup(motor_control_t *cmotor, float flow, float fhigh);

/**
 * @brief: Function checking back-emf stability
 * @note: It should rely on samples taken between 2 points in the EMF.
 */
int mcontrol_stable_check(motor_control_t *cmotor);

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


/**
 * @brief: Function used to read the inactive phase
 */
int mcontrol_read_phase(motor_control_t *cmotor, enum phase ph);

#endif