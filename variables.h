#ifndef VARIABLES_H
#define VARIABLES_H

/*
 * Controls the Serial baud rate 
 */
const int VAR_serial_baud = 9600;

/*
 * Controls if the Arduino should wait for a serial connection before booting
 */
const bool VAR_serial_wait = false;

/*
 * Controls how far contrllers with analog triggers have to be pressed before they trigger
 * a mapped PSP button.
 * 
 * Note analog triggers are referred to as brake (Left side) and throttle (Right side) in the mappings.
 * 
 * This is for controllers like the Xbox one which have a button (L1) and trigger,
 * unlike the PS4 which has two buttons (L1, L2 etc)
 * 
 * Range: 0 - 1023
 */
const int VAR_analog_trigger_threshold = 300;

/*
 * Controls how far the analog stick has to move from the centre position to trigger
 * a mapped PSP button.
 * 
 * Range: 0 - 511
 */
const int VAR_ts_btn_threshold = 250;

#endif
