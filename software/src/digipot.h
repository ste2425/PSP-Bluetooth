#ifndef DIGIPOT_H
#define DIGIPOT_H

#include <Arduino.h>
#include "MCP_POT.h"
#include <SPI.h>

void DIGIPOT_write_x(int value);
void DIGIPOT_write_y(int value);
void DIGIPOT_setup();

#endif
