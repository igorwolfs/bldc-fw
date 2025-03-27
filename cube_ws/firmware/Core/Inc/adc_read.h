#ifndef ADC_READ_H
#define ADC_READ_H
#include "usb_device.h"

void adc_read_vbat(float *ret);
void adc_read_temp(float *ret);


void adc1_read(int ret_idx, float *ret);
void adc2_read(int ret_idx, float *ret);
void adc3_read(int ret_idx, float *ret);
void adc4_read(int ret_idx, float *ret);


#endif