# bldc-fw

Code containing the control firmware used along with the [BLDC-motor](https://github.com/igorwolfs/bldc-hw)-hardware.

## Cube
The [cube_ws](/cube_ws/)-directory contains firmware generated using the stm32cube-suite, including bringup-code for the ADC, timers and USB CDC device.

## Zephyr
The [zephyr_ws](/zephyr_ws/)-directory contains firmware written for the Zephyr-RTOS. 

In order to fetch [the zephyr device-tree](https://github.com/igorwolfs/zephyr-boards), make sure to run 
```bash
west update
```
Inside the [zephyr_ws](/zephyr_ws/)-directory.
