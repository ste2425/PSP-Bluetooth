#include "Mappings.h"
#include <ArduinoJson.h>

const char defaultJSON[] = "[{\"n\":1,\"c\":[255,0,0,0.1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[0,1,3]]},{\"n\":2,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[102,6,2],[101,7,2]]}]";

MappingMeta test[10];
MappingMeta *MAPPINGS_current;
uint8_t maxMappings = 1; // zero indexed
uint8_t currentMapping = 0;

void MAPPINGS_setup() {
  
  JsonDocument doc;

  String savedMappings = FileUtility::readFile(LittleFS, "/mapping.json");
  bool isEmpty = savedMappings == "";

  if (isEmpty) {
    Serial.println("Loading default");
    deserializeJson(doc, defaultJSON, strlen(defaultJSON));
  } else {
    Serial.println("Loading Saved");
    auto len = savedMappings.length() + 1;
    char dataArr[len];
    savedMappings.toCharArray(dataArr, len);
    deserializeJson(doc, dataArr, len);
  }
  
  JsonArray mappingsArray = doc.as<JsonArray>();
  uint8_t mappingsLength = mappingsArray.size();

  maxMappings = mappingsLength;

  for (int mI = 0; mI < 10; mI++) {
    if (mI < mappingsLength) {
      test[mI].isSet = true;
      test[mI].mappingNumer = (uint8_t)mappingsArray[mI]["n"];
      
      JsonArray c = mappingsArray[mI]["c"];

      uint8_t r = c[0];
      uint8_t g= c[1];
      uint8_t b = c[2];
      float s = c[3];
      
      test[mI].colour[0] = round(r * s);
      test[mI].colour[1] = round(g * s);
      test[mI].colour[2] = round(b * s);
      
      JsonArray m = mappingsArray[mI]["m"];
      uint8_t arrayLen = m.size();
  
      for (int i = 0; i < 30; i++) {
        if (arrayLen < 30) {
          test[mI].mappings[i] = {
            m[i][0], m[i][1], m[i][2]
          };
        } else {
          test[mI].mappings[i] = {99};
        }
      }
    } else {
      test[mI].isSet = false;
    }
  }

  MAPPINGS_current = &test[0];
}

void MAPPINGS_next() {
  currentMapping++;
  
  if (currentMapping >= maxMappings) {
    currentMapping = 0;  
  }

  MAPPINGS_current = &test[currentMapping];
}
