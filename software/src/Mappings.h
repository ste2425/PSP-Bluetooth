#ifndef MAPPING_H
#define MAPPING_H

#define MAPPINGS_DEFAULT "[{\"n\":1,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[0,1,3]]},{\"n\":2,\"c\":[0,255,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[102,6,2],[101,7,2]]}]"

#include "MappingTypes.h"
#include "PSPState.h"
#include <Arduino.h>
#include <LittleFS.h>
#include "fileUtility.h"

extern MappingMeta *MAPPINGS_current;

void MAPPINGS_next();
void MAPPINGS_setup();

#endif
