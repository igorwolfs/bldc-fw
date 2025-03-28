#ifndef ADC_READ_H
#define ADC_READ_H
#include "usb_device.h"


void adc_vref_read(void);

void adc_read_vbat(float *ret);
void adc_read_temp(float *ret);


// READ VPHASE
void adc_read_vphase_u(float *ret);
void adc_read_vphase_v(float *ret);
void adc_read_vphase_w(float *ret);
void adc_read_vphase_n(float *ret);

// READ IPHASE
void adc_read_iphase_u(float *ret);
void adc_read_iphase_v(float *ret);
void adc_read_iphase_w(float *ret);

// READ ADC
void adc1_read(int ret_idx, float *ret);
void adc2_read(int ret_idx, float *ret);
void adc3_read(int ret_idx, float *ret);
void adc4_read(int ret_idx, float *ret);

#endif