#include "phase_read.h"
#include "motor_control.h"
#include "main.h"


int read_phase_u(motor_control_t *mcontrol, enum phase ph)
{
	phase_read_t *ret = mcontrol->phase_data[ph];
	float vphase, vstar, iphase;
	switch (ph)
	{
		case (PHASE_U):
			adc1_read(VPHASE_U_ADC1_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			ret->emf = vphase - vstar;
			if (mcontrol->control_type == FOC)
			{
				// Measure current here as well
				adc3_read(IPHASE_U_ADC3_IDX, &ret->current);
			}
			break;
		case (PHASE_V):
			adc2_read(VPHASE_V_ADC2_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			ret->emf = vphase - vstar;
			if (mcontrol->control_type == FOC)
			{
				// Measure current here as well
				adc4_read(IPHASE_V_ADC4_IDX, &ret->current);
			}
			break;
		case (PHASE_W):
			adc2_read(VPHASE_W_ADC2_IDX, &vphase); // Second data from buffer
			adc1_read(VPHASE_N_ADC1_IDX, &vstar);
			ret->emf = vphase - vstar;
			if (mcontrol->control_type == FOC)
			{
				// Measure current here as well
				adc2_read(IPHASE_W_ADC2_IDX, &ret->current);
			}
			break;
	}
}




/**
 *
 * * start with a simple example and a stupid sequence where everything is always measured always.
 * * Check the basic control strategy, and optimize after that.
 * * In the future: simply set a timer to measure all adc values at timer trigger
 */