#ifndef CONTROLLERSYNC_H
#define CONTROLLERSYNC_H

#include <Arduino.h>
#include <Bluepad32.h>
#include <timer.h>

void CTRSYNC_start();
void CTRSYNC_stop();
void CTRSYNC_setup();
boolean CTRSYNC_isActive();

#endif
