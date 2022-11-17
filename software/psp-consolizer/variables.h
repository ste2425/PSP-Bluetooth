#ifndef VARIABLES_H
#define VARIABLES_H

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
 * Default: 300
 */
const int VAR_analog_trigger_threshold = 300;

/*
 * Controls how far the analog stick has to move from the centre position to trigger
 * a mapped PSP button.
 * 
 * Range: 0 - 511
 * Default: 250
 */
const int VAR_ts_btn_threshold = 250;

/*
 * Controls how long to wait in milliseconds after turning the PSP on before pressing the display for TV out. 
 * 
 * If pressed too soon the console will ignore it.
 * 
 * This value was achieved through trial and error. It gives reliable use on my console but others may take longer to boot.
 * Memory card used space, custom firmware types, soft vs hard boot could all affect how long it takes before the PSP will listen
 * to the screen button.
 * 
 * VAR_boot_screen_wait Default: 12000
 * VAR_boot_screen_wait_warm Default: 1500
 */
const int VAR_boot_screen_wait = 12000;
const int VAR_boot_screen_wait_warm = 1500;

/**
* If enabled the display button can be manually controlled
*
* Clicking the right analog stick in and pressing DPAD left will press the display button
*
* Defualt: false
*/
const bool VAR_enable_manual_display_control = true;

#endif
