#ifndef POT_H
#define POT_H

#include <Arduino.h>
#include <SPI.h>

void POT_write_X(int value);
void POT_write_Y(int value);
void POT_set(int x, int y);
uint8_t POT_map_controller(int controllerVal);
void POT_centre();
void POT_setup();

#endif
