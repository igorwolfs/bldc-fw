## DEBUGGING
These are for some reason the kind-of values that I get:

### Voltage is 1 V
ADC1: gvirt: [224, 0.57], ADC1: U_div: [80, 0.20], ADC1: temp_int: [1683, 1.42], ADC1: vref_int: [1454, 1.23], 
ADC2: V_div: [311, 0.79], ADC2: W_div: [307, 0.78], ADC2: W_current: [0, 0.00], 
ADC3: U_current: [0, 0.00], 
ADC4: V_current: [0, 0.00], ADC4: temp_ext: [2355, 1.99], ADC4: vbat_ext: [45, 0.42], 

### Voltage is 2 V
ADC1: gvirt: [514, 1.31], ADC1: U_div: [225, 0.57], ADC1: temp_int: [1683, 1.42], ADC1: vref_int: [1454, 1.23], 
ADC2: V_div: [675, 1.71], ADC2: W_div: [674, 1.71], ADC2: W_current: [0, 0.00], 
ADC3: U_current: [0, 0.00], 
ADC4: V_current: [0, 0.00], ADC4: temp_ext: [2352, 1.99], ADC4: vbat_ext: [157, 1.46],

### Voltage is 5 V
ADC1: gvirt: [1495, 3.53], ADC1: U_div: [719, 1.70], ADC1: temp_int: [1810, 1.42], ADC1: vref_int: [1565, 1.23], 
ADC2: V_div: [1905, 4.49], ADC2: W_div: [1900, 4.48], ADC2: W_current: [0, 0.00], 
ADC3: U_current: [0, 0.00], 
ADC4: V_current: [0, 0.00], ADC4: temp_ext: [2349, 1.85], ADC4: vbat_ext: [532, 4.60], 

### Voltage is 7 V
ADC1: gvirt: [1977, 5.02], ADC1: U_div: [961, 2.44], ADC1: temp_int: [1683, 1.42], ADC1: vref_int: [1454, 1.23], 
ADC2: V_div: [2511, 6.38], ADC2: W_div: [2511, 6.38], ADC2: W_current: [0, 0.00], 
ADC3: U_current: [0, 0.00], 
ADC4: V_current: [0, 0.00], ADC4: temp_ext: [2350, 1.99], ADC4: vbat_ext: [718, 6.68], 

### Voltage is 9 V
ADC1: gvirt: [2565, 6.51], ADC1: U_div: [1257, 3.19], ADC1: temp_int: [1683, 1.42], ADC1: vref_int: [1454, 1.23], 
ADC2: V_div: [3251, 8.25], ADC2: W_div: [3248, 8.25], ADC2: W_current: [0, 0.00], 
ADC3: U_current: [0, 0.00], 
ADC4: V_current: [0, 0.00], ADC4: temp_ext: [2350, 1.99], ADC4: vbat_ext: [947, 8.82], 

### Current values
- The current values are 0 everywhere, this is in line with what is expected.

### Reference voltage
There seems to be an offset between the reference voltage and the actual measured voltage (measured is 3.27, adc output is 3.46 V).

## Calibration
After calibration the reference and battery voltage seem to be extremely close to the desired voltage.

However there is still an offset for the phase voltages somehow, this might be due to
- Voltage drops over the wire
	- Measuring with an oscilloscope close to the oscilloscope

e.g.:  
#### NPHASE measurement
- Ground measurement is 1.24 Volts at adc input
- ADC value: 1585
- Reference: 3.32 V
- So (1585 /4096) * 3.32 = 1.284 volt -> so that's correct
- Now multiplied by 3 -> 1.284*3 = 3.852 Volt neutral voltage

#### UPHASE measurement
- 0.6 V actual phase voltage
- 0.64 volt at adc input
- 808 in ADC
- (808 / 4095) * 3.32 = 0.6551 V
- Multiplied by 3 should be: 0.6551*3 = 1.965 V

Weird,  check the resistive divider value at the phase.
- This one is simply 600 mV across the board because there's no ground connection.

#### VPHASE measurement
- 5.52 V (VBAT as expected)
- VPHASE: 2017
- VREF: 3.32
- (2017 / 4095) * 3.32 = 1.635 -> * 3 = 4.906 V

Check the actual voltage at the ADC to see if this is a voltage drop issue
- Measured there is something between 1.62 and 1.64 V, so that's correct.
The voltage drop is the only thing we have to take into account here really.


### Inaccuracies at the resistors
- Their tolerance is about +-1 %
- So the max deviation is about (1+d0.01) / (3+d0.03)

### Wire voltage drops
- 640 - 680 mV at the input
- 600 - 640 mV at the actual phase
So it seems like there is a voltage drop

## Battery voltage
- The battery voltage starts very close to 0, and increases linearly with what is expected.
- The battery voltage factor should be (33 / (330+33)) = 1/11

e.g.:
- vbat = 932
- (932 / 4096) * 1454 = 

## TODO
- Fix the ADC measurements
-> DONE

## Next steps 
- Run the motor at fixed low frequency (at about 10 V)
- Check the ADC values at the same time
	- Perhaps think about running continuous conversion with the ADC
	- First try without the ADC measurement of the un-activated phase.

## Issues
- High current draw of the motor

### Debugging
- Keep the switching frequency constant
	- Check if the BLDC at a certain point keeps spinning constantly when enabling high current draw.
	- If this isn't happening, check the voltage waveforms.

#### Checking if the switching circuit does what it is supposed to
- Electrical switching (W_SD) happens at 16 Hz
- Electrical switching (W_SW) happens at 8 Hz

The phase switches on and off once every electrical cycle.
- So each electrical cycle is about 8 Hz.
- So 8 Hz / 4 pole pairs = 2 Hz rotational frequency
	- Maybe this is already somewhat to fast? Maybe we need to start with 1 Hz and check where that brings us?
	- Or maybe it's too slow for a no-load situation


#### Checking the phase switching waves, and whether they correspond to 8 Hz
- HIGH: 8 Hz switching speed
- LOW: 8 Hz switching speed
-> Looks good

- The core issue though seems we simply need more data and we don't have it.

## Change starting approach
- Try starting at 30 rpm with quick ramp-up until 200 rpm
- Try aligning the rotor phases first so the location is known.


### Motor alignment
- Change the prescaler to 7
- Align the motor from the start by driving phase (W+, V-), or (W-)
	- This should align you in a good position for step 1.
	- Then start the motor at an RPM of 30 (1/2 rps)
- Ramp up quickly to 200 rpm.

- PWM:
	- Make sure to drive the signals here in PWM-fashion.
	- There needs to be only 3 PWM signals, which enable the (SD)
	- So PHASE_U_SD, PHASE_V_SD, PHASE_W_SD
		- PHASE_U_SD: PB0
		- PHASE_V_SD: PB6
		- PHASE_W_SD: PB4
	- These ones should have a HF PWM signal which we should be able to start / stop and set the duty cycle of.