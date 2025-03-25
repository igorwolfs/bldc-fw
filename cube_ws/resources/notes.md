# Building and Flashing
## Building
```bash
sudo apt install gcc-arm-none-eabi
```
## Flashing
1. Download stm32-cube programmer.

2. Use the following command to flash the stm32f303, which performs a hardware reset
```bash
$STM32_PRG_PATH/STM32_Programmer_CLI -c port=swd mode=UR -rst -w <path_to_binary> 0x08000000
```
with STM32_PRG_PATH the path to the CLI binary.

In case of issues, perform a flash erase:
```bash
$STM32_PRG_PATH/STM32_Programmer_CLI -c port=swd mode=UR -e all
```

# Sensorless Control
## Step 1: Startup
**Goal**
Detect enough movement so the adc can detect movement and we can jump to the control step.

This phase keeps running, the speed lock should probably store some historic data to see the back-emf sequence, and check if it's actually corresponding to a trapezoidal curve.

## Step 2: Speed lock
**Concept**
Simple sensorless control based on back-emfs.
Given the "approached" back emf (phase voltage - virtual ground), we know 
- How big the back-emf is (approximately)
- When the back-emf crosses zero

- One phase is not driven during each of the 6 steps.
- In that phase a back-emf is induced which relates to the existing speed as referenced in [equations](https://github.com/igorwolfs/bldc-hw/blob/main/rev_1/resources/motor.ipynb) (driving voltage and di/dt drops away)
- So the back-emf becomes f(theta_electrical).
	- Zero crossing means the rotor is passing the coil (so its halfway)
	- Wait for the switching period / 2 after that to switch again.

**Steps**

1. Sample the zero-crossing of the non-energised phase
2. Wait half a step-period
3. Switch to the next step


# Speed
## Timer speed
- Timer is fed with a 48 MHz clock
- Pole pair number: 4
- Max timer count: 65535
- Prescaler (settable)
- 6 electrical steps

Minimum mechanical speed becomes
- switching_period = 65535 / (48e6) = 1.365 ms
- mech_speed = 1/(1.365e-3 * 4 * 6) = 30.5 rps = 1830 rpm (which is too large)

Choose an input clock of 6 MHz instead so the min starting rpm becomes 

## 