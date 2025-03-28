#include <stdint.h>
#include <math.h>
#include "adc_read.h"
#include "main.h"

/**
 * 
 * typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;
 */

// #define ADC_DEBUG
// *** SYSTEM VOLTAGES ***

#define ADC4_TEMP_IDX   1
#define ADC4_VBAT_IDX   2
#define ADC1_VREF_IDX   2

void adc_vref_read(void)
{
    float ret;
    adc1_read(ADC1_VREF_IDX, &ret);
}

void adc_read_vbat(float *ret) {
    adc4_read(ADC4_VBAT_IDX, ret);
}

void adc_read_temp(float *ret) {
    adc4_read(ADC4_TEMP_IDX, ret);
}

// *** PHASE VOLTAGES ***
#define VPHASE_U_ADC1_IDX	1
#define VPHASE_V_ADC2_IDX	0
#define VPHASE_W_ADC2_IDX	1
#define VPHASE_N_ADC1_IDX	0

void adc_read_vphase_u(float *ret) {
    adc1_read(VPHASE_U_ADC1_IDX, ret);
}
void adc_read_vphase_v(float *ret) {
    adc2_read(VPHASE_V_ADC2_IDX, ret);
}
void adc_read_vphase_w(float *ret) {
    adc2_read(VPHASE_W_ADC2_IDX, ret);
}
void adc_read_vphase_n(float *ret) {
    adc1_read(VPHASE_N_ADC1_IDX, ret);
}

// *** PHASE CURRENTS ***
#define IPHASE_U_ADC3_IDX	0
#define IPHASE_V_ADC4_IDX	0
#define IPHASE_W_ADC2_IDX	2

void adc_read_iphase_u(float *ret) {
    adc3_read(IPHASE_U_ADC3_IDX, ret);
}
void adc_read_iphase_v(float *ret) {
    adc4_read(IPHASE_V_ADC4_IDX, ret);
}
void adc_read_iphase_w(float *ret) {
    adc2_read(IPHASE_W_ADC2_IDX, ret);
}

/****
 * ADCi_READ FUNCTIONS
 * */
/**
 * ADC1_CH2: gvirtual
 * ADC1_CH3: PhaseU_DIV
 * ADC1_VREF
 */

 static float VDDA_ref = -1;
 
 extern ADC_HandleTypeDef hadc1;
 extern ADC_HandleTypeDef hadc2;
 extern ADC_HandleTypeDef hadc3;
 extern ADC_HandleTypeDef hadc4;

// VOLTAGE MEASUREMENT CALIBRATION
#define VPHASE_CALIB_FACTOR   (1.0 / 3.0)
#define IPHASE_CALIB_FACTOR   (0.003 * 50.0) 


// TEMPERATURE CALIBRATION
#define TS_CAL1_ADDR       ((uint16_t*)0x1FFFF7B8) // Calibration value at 30°C
#define TS_CAL2_ADDR       ((uint16_t*)0x1FFFF7C2) // Calibration value at 110°C

// * (0x1FFF F7BA, 0x1FFF F7BB)
#define VREFINT_CAL_ADDR   ((uint16_t*)0x1FFFF7BA) // VREFINT calibration value
#define VREF               3.3f   // VREF value (p. 381 user manual)
#define TEMP30_CAL_DEG     30.0f  // Temperature at TS_CAL1
#define TEMP110_CAL_DEG    110.0f // Temperature at TS_CAL2

#define N_CONVERTS_ADC1  3

static void adc_vref_update(uint16_t VDDA_ui16)
{
    // VDDA Calculation
    volatile uint16_t vref_cal = *VREFINT_CAL_ADDR;
    //! WARN: make sure to divide the uint16's as floats.
    VDDA_ref = VREF *  ((float)((float)vref_cal / (float)VDDA_ui16));
}


void adc1_read(int ret_idx, float *ret)
{

    uint16_t adc_buffer[N_CONVERTS_ADC1] = {0};
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        printf("[ERR] ADC Start Error\n");
        return;
    }

    for (int i=0; i<N_CONVERTS_ADC1; i++)
    {
        HAL_ADC_PollForConversion(&hadc1, 1000);
        adc_buffer[i] = HAL_ADC_GetValue(&hadc1);
    }

    HAL_ADC_Stop(&hadc1);
    adc_vref_update(adc_buffer[ADC1_VREF_IDX]);
    switch (ret_idx)
    {
        case VPHASE_U_ADC1_IDX: case VPHASE_N_ADC1_IDX:
            *ret = VDDA_ref * ((float)((float)adc_buffer[ret_idx]) / (float)0xfff) / VPHASE_CALIB_FACTOR;
            break;
    }
    #ifdef ADC_DEBUG
    char* adc_naming[] = {"gvirt", "U_div", "vref_int"};
    int adc_periph=1;
    float fp_factor[] = {VPHASE_CALIB_FACTOR, VPHASE_CALIB_FACTOR, 1};
    for (int i=0; i<N_CONVERTS_ADC1; i++)
    {
        float adc_fp = VDDA_ref * ((float)((float)adc_buffer[i]) / (float)0xfff) / fp_factor[i];
        printf("ADC%d: %s: [%u, %.2f], ", adc_periph, adc_naming[i], adc_buffer[i], adc_fp);
    }
    printf("VREF_INT: %.2f", VDDA_ref);
    printf("\r\n");
    #endif
}

/**
 * ADC2_CH2: PhaseV_DIV
 * ADC2_CH3: PhaseW_DIV
 * ADC2_CH12: PhaseW-current
 */

#define N_CONVERTS_ADC2  3
void adc2_read(int ret_idx, float *ret)
{
    uint16_t adc_buffer[N_CONVERTS_ADC2] = {0};
    if (HAL_ADC_Start(&hadc2) != HAL_OK)
    {
        printf("[ERR] ADC Start Error\n");
        return;
    }

    for (int i=0; i<N_CONVERTS_ADC2; i++)
    {
        HAL_ADC_PollForConversion(&hadc2, 1000);
        adc_buffer[i] = HAL_ADC_GetValue(&hadc2);
    }

    HAL_ADC_Stop(&hadc2);
    switch (ret_idx)
    {
    case VPHASE_V_ADC2_IDX: case VPHASE_W_ADC2_IDX:
        *ret = VDDA_ref * ((float)((float)adc_buffer[ret_idx]) / (float)0xfff) / VPHASE_CALIB_FACTOR;
        break;
    case IPHASE_W_ADC2_IDX:
        *ret = VDDA_ref * ((float)((float)adc_buffer[IPHASE_W_ADC2_IDX]) / (float)0xfff) / IPHASE_CALIB_FACTOR;
        break;
    }

    #ifdef ADC_DEBUG
    char* adc_naming[] = {"V_div", "W_div", "W_current"};
    int adc_periph=2;
    float fp_factor[] = {VPHASE_CALIB_FACTOR, VPHASE_CALIB_FACTOR, IPHASE_CALIB_FACTOR};
    for (int i=0; i<N_CONVERTS_ADC2; i++)
    {
        float adc_fp = VDDA_ref * ((float)((float)adc_buffer[i]) / (float)0xfff) / fp_factor[i];
        printf("ADC%d: %s: [%u, %.2f], ", adc_periph, adc_naming[i], adc_buffer[i], adc_fp);
    }
    printf("\r\n");
    #endif
}

#define N_CONVERTS_ADC3  1
// Phase U-current: PB13, ADC3_IN5
void adc3_read(int ret_idx, float *ret)
{
    uint16_t adc_buffer[N_CONVERTS_ADC3] = {0};
    if (HAL_ADC_Start(&hadc3) != HAL_OK)
    {
        printf("[ERR] ADC Start Error\n");
        return;
    }

    for (int i=0; i<N_CONVERTS_ADC3; i++)
    {
        HAL_ADC_PollForConversion(&hadc3, 1000);
        adc_buffer[i] = HAL_ADC_GetValue(&hadc3);
    }

    HAL_ADC_Stop(&hadc3);
    switch (ret_idx)
    {
        case (IPHASE_U_ADC3_IDX):
            *ret = VDDA_ref * ((float)((float)adc_buffer[ret_idx]) / (float)0xfff) / IPHASE_CALIB_FACTOR;
            break;
    }

    #ifdef ADC_DEBUG
    char* adc_naming[] = {"U_current"};
    float fp_factor[] = {IPHASE_CALIB_FACTOR};
    int adc_periph=3;
    for (int i=0; i<N_CONVERTS_ADC3; i++)
    {
        float adc_fp = VDDA_ref * ((float)((float)adc_buffer[i]) / (float)0xfff) / fp_factor[i];
        printf("ADC%d: %s: [%u, %.2f], ", adc_periph, adc_naming[i], adc_buffer[i], adc_fp);
    }
    printf("\r\n");
    #endif
}

/**
 * Channel 4: V_current, temperature, vbat
 * ? Temperature
 * We need the temperature as a function of R2
 * 
 */

// * TEMPERATURE
#define THERMISTOR_B    4050.0
#define THERMISTOR_TCAL1 298.15
#define THERMISTOR_RBASE   47.0 //kOhm
#define THERMISTOR_R1   33.0 // kOhm

// * Battery voltage
#define BATTMEAS_RATIO (33.0/(330.0+33.0)) // kOhm


#define N_CONVERTS_ADC4  3
void adc4_read(int ret_idx, float *ret)
{
    uint16_t adc_buffer[N_CONVERTS_ADC4] = {0};
    if (HAL_ADC_Start(&hadc4) != HAL_OK)
    {
        printf("[ERR] ADC Start Error\n");
        return;
    }

    for (int i=0; i<N_CONVERTS_ADC4; i++)
    {
        HAL_ADC_PollForConversion(&hadc4, 1000);
        adc_buffer[i] = HAL_ADC_GetValue(&hadc4);
    }

    HAL_ADC_Stop(&hadc4);
    switch (ret_idx)
    {
        case ADC4_VBAT_IDX:
            *ret = VDDA_ref * ((float)adc_buffer[ADC4_VBAT_IDX] / (float)0xfff) / BATTMEAS_RATIO;
            // printf("meas: %.3f, ret: %.3f\r\n", meas, *ret);
            break;
        case ADC4_TEMP_IDX:
            // * Temperature calculation using thermistor
            //! Warn: the VDDA_ref here comes from the first adc, we should use the one that is used as input for adc3 and 4.
            float v_temp = VDDA_ref * ((float)((float)adc_buffer[ADC4_TEMP_IDX] / (float)0xfff));
            // Current through primary resistor
            float th_ir1 = (VDDA_ref - v_temp) / THERMISTOR_R1;
            // Divide v_temp / current through primary
            float thermistor_rnew = v_temp / th_ir1;
            // Calculate the temperature based on the 2 resistors
            float T2_val_inv = 1/THERMISTOR_TCAL1 - logf(THERMISTOR_RBASE/thermistor_rnew) / THERMISTOR_B;
            float T2_val = 1/T2_val_inv - 273.15;
            *ret = T2_val;
            break;
        case IPHASE_V_ADC4_IDX:
            *ret = VDDA_ref * ((float)((float)adc_buffer[ret_idx]) / (float)0xfff) / IPHASE_CALIB_FACTOR;
            break;
    }
    #ifdef ADC_DEBUG
    int adc_periph=4;
    char* adc_naming[] = {"V_current", "temp_ext", "vbat_ext"};
    float fp_factor[] = {IPHASE_CALIB_FACTOR, 1, BATTMEAS_RATIO};
    for (int i=0; i<N_CONVERTS_ADC4; i++)
    {
        float adc_fp = VDDA_ref * ((float)((float)adc_buffer[i]) / (float)0xfff) / fp_factor[i];
        printf("ADC%d: %s: [%u, %.2f], ", adc_periph, adc_naming[i], adc_buffer[i], adc_fp);
    }
    printf("\r\n");
    #endif
}