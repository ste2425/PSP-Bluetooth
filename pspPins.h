#ifndef PSPPINS_H
#define PSPPINS_H

#include <Arduino.h>
#include <stdint.h>
#include "digipot.h"

extern const uint8_t PSP_HOME;
extern const uint8_t PSP_START;
extern const uint8_t PSP_SELECT;
extern const uint8_t PSP_DISPLAY;
extern const uint8_t PSP_L1;
extern const uint8_t PSP_DUP;
extern const uint8_t PSP_DDOWN;
extern const uint8_t PSP_DRIGHT;
extern const uint8_t PSP_DLEFT;
extern const uint8_t PSP_R1;
extern const uint8_t PSP_CROSS;
extern const uint8_t PSP_CIRCLE;
extern const uint8_t PSP_SQUARE;
extern const uint8_t PSP_TRIANGLE;
extern const uint8_t PSP_POWER;
extern const uint8_t PSP_LS_LEFT;
extern const uint8_t PSP_LS_RIGHT;
extern const uint8_t PSP_LS_UP;
extern const uint8_t PSP_LS_DOWN;
extern const uint8_t PSP_LS;

void PSP_mark_all_for_release();
void PSP_release_unused();
void PSP_press_button(uint8_t pspButton);
void PSP_power_on();
void PSP_power_off();
void PSP_toggle_screen();
void PSP_set_ls(uint8_t x, uint8_t y);
uint8_t PSP_map_controller(int controllerVal);
void toggle_power();

#endif
