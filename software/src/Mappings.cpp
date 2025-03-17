#include "Mappings.h"
#include <ArduinoJson.h>

MappingMeta test[10];
MappingMeta *MAPPINGS_current;
uint8_t maxMappings = 1; // zero indexed
uint8_t currentMapping = 0;

void MAPPINGS_setup() {
  JsonDocument doc;

  Serial.println("Loading Saved");
  String savedMappings = FileUtility::readFile(LittleFS, "/mapping.json");
  Serial.println("Loaded");
  Serial.println(savedMappings);

  auto len = savedMappings.length() + 1;
  char dataArr[len];
  savedMappings.toCharArray(dataArr, len);

  deserializeJson(doc, dataArr, len);

  JsonArray mappingsArray = doc.as<JsonArray>();
  uint8_t mappingsLength = mappingsArray.size();

  if (mappingsLength == 0) {  
    Serial.println("Saved empty: Loading default");  
    doc = NULL;
    deserializeJson(doc, MAPPINGS_DEFAULT, strlen(MAPPINGS_DEFAULT));

    mappingsArray = doc.as<JsonArray>();
    mappingsLength = mappingsArray.size();
  }

  maxMappings = mappingsLength;

  Serial.print("max mappings: ");
  Serial.println(maxMappings);

  for (int mI = 0; mI < 10; mI++) {
    if (mI < mappingsLength) {
      Serial.print("Processing controller at index: ");
      Serial.println(mI);

      test[mI].isSet = true;
      test[mI].mappingNumer = (uint8_t)mappingsArray[mI]["n"];

      Serial.print("Controller number: ");
      Serial.println(test[mI].mappingNumer);
      
      JsonArray c = mappingsArray[mI]["c"];

      uint8_t r = c[0];
      uint8_t g= c[1];
      uint8_t b = c[2];
      float s = c[3];
      
      test[mI].colour[0] = round(r * s);
      test[mI].colour[1] = round(g * s);
      test[mI].colour[2] = round(b * s);

      Serial.print("Controller Colour: (");
      Serial.print(test[mI].colour[0]);
      Serial.print(",");
      Serial.print(test[mI].colour[1]);
      Serial.print(",");
      Serial.print(test[mI].colour[2]);
      Serial.println(")");
      
      JsonArray m = mappingsArray[mI]["m"];
      uint8_t arrayLen = m.size();
  
      for (int i = 0; i < 30; i++) {
        if (i < arrayLen) {
          test[mI].mappings[i] = {
            m[i][0], m[i][1], m[i][2]
          };
        } else {
          test[mI].mappings[i] = {
            99, 0, 0
          };
        }
      }
    } else {
      test[mI].isSet = false;
    }
  }

  if (currentMapping >= maxMappings) {
    Serial.println("Resettig to zero");
    currentMapping = 0;
  }

  MAPPINGS_current = &test[currentMapping];
}

void MAPPINGS_next() {
  currentMapping++;
  
  Serial.print("Next mapping, current: ");
  Serial.print(currentMapping);
  Serial.print("Max: ");
  Serial.println(maxMappings);
  
  if (currentMapping >= maxMappings) {
    Serial.println("Resetting mappings");
    currentMapping = 0;  
  }

  MAPPINGS_current = &test[currentMapping];
}
