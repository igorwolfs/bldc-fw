#include "motor_control.h"
#include "main.h"
#include "adc_read.h"


#define VPHASE_U_ADC1_IDX	1
#define VPHASE_V_ADC2_IDX	0
#define VPHASE_W_ADC2_IDX	1
#define VPHASE_N_ADC1_IDX	0

#define IPHASE_U_ADC3_IDX	0
#define IPHASE_V_ADC4_IDX	0
#define IPHASE_W_ADC2_IDX	2


int mcontrol_read_phase(motor_control_t *cmotor, enum phase ph)
{
	phase_read_t *ret = cmotor->phase_data[ph];
	float vphase, vstar;
	switch (ph)
	{
		case (PHASE_U):
			adc1_read(VPHASE_U_ADC1_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			if (cmotor->control_type == FOC)
			{
				// Measure current here as well
				adc3_read(IPHASE_U_ADC3_IDX, &ret->current);
			}
			break;
		case (PHASE_V):
			adc2_read(VPHASE_V_ADC2_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			if (cmotor->control_type == FOC)
			{
				// Measure current here as well
				adc4_read(IPHASE_V_ADC4_IDX, &ret->current);
			}
			break;
		case (PHASE_W):
			adc2_read(VPHASE_W_ADC2_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			if (cmotor->control_type == FOC)
			{
				// Measure current here as well
				adc2_read(IPHASE_W_ADC2_IDX, &ret->current);
			}
			break;
	}
	// SAVE DATA TO ARRAY (stability + measurement)
	ret->emf = vphase - vstar;
	int hist_arr_idx = 2 * cmotor->ecycle_count * (cmotor->inv->state+1);
	if (cmotor->adc_step == MCONTROL_ADC_STEP1 || cmotor->adc_step == MCONTROL_ADC_STEP2)
	{
		cmotor->hist_arr[hist_arr_idx] = ret->emf;
	}
	else if (cmotor->adc_step == MCONTROL_ADC_STEP2)
	{
		cmotor->hist_arr[hist_arr_idx + 1] = ret->emf;
	}
}




/**
 *
 * * start with a simple example and a stupid sequence where everything is always measured always.
 * * Check the basic control strategy, and optimize after that.
 * * In the future: simply set a timer to measure all adc values at timer trigger
 */