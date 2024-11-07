#ifndef MAPPING_H
#define MAPPING_H

#include "MappingTypes.h"
#include "PSPState.h"
#include <Arduino.h>
#include <LittleFS.h>
#include "fileUtility.h"

extern MappingMeta *MAPPINGS_current;

void MAPPINGS_next();
void MAPPINGS_setup();

#endif
