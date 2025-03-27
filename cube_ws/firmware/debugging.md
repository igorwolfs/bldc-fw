# DEBUGGING
These are for some reason the kind-of values that I get:

ADC1: gvirt: 2535, ADC1: U_div: 1242, ADC1: temp_int: 1685, ADC1: vref_int: 1455, 
ADC2: V_div: 3198, ADC2: W_div: 3202, ADC2: W_current: 0, 
ADC3: W_current: 0, 
ADC4: V_current: 0, ADC4: temp_ext: 2390, ADC4: vbat_ext: 928, 


## Current values
- The current values are 0 everywhere, this is in line with what is expected.

## Battery voltage
- The battery voltage starts very close to 0, and increases linearly with what is expected.
- The battery voltage factor should be (33 / (330+33)) = 1/11

e.g.:
- vbat = 932
- (932 / 4096) * 