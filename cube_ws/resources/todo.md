# Architectural todo
- Initialize control type
- Initialize startup type
- in the init
	- Initialize relevant adc, timer and other variables
	- e.g.: start with sensorless control

### ADC measurements
- Create a function for updating the VDDA_ref
- Calculate it everyt ime adc1 is called

### ADC measurements combined with driving
- The ADC will be driven by the same timer interrupt as the inverter.
- The ADC however will be triggered every time.
- Why not a separate timer? Because we might get in trouble there with different periods and triggering times + different starting times.
	- If we want to accurately be on the same edge as the switching, we need to put the ADC and the inverter on the same timer.
	