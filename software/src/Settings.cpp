#include "Settings.h"
#include <ArduinoJson.h>

Settings SETTINGS_current;

void SETTINGS_setup() {
  JsonDocument doc;

  Serial.println("Loading Settings Saved");
  String savedMappings = FileUtility::readFile(LittleFS, "/settings.json");
  auto len = savedMappings.length() + 1;
  char dataArr[len];
  savedMappings.toCharArray(dataArr, len);

  deserializeJson(doc, dataArr, len);

  if (doc.isNull()) {  
    Serial.println("Settings Saved empty: Loading default");  
    doc = NULL;
    deserializeJson(doc, SETTINGS_DEFAULT, strlen(SETTINGS_DEFAULT));
  }

  SETTINGS_current.useExtraCombo = (bool)doc["useExtraCombo"];
  SETTINGS_current.ledBrightness = (uint16_t)doc["ledBrightness"];
  SETTINGS_current.throttleThreshold = (uint16_t)doc["throttleThreshold"];
  SETTINGS_current.analogThreshold = (uint16_t)doc["analogThreshold"];
  SETTINGS_current.diagonalThreshold = (uint16_t)doc["diagonalThreshold"];
  SETTINGS_current.screenDelay = (uint16_t)doc["screenDelay"];
  SETTINGS_current.coldBootDelay = (uint16_t)doc["coldBootDelay"];
  SETTINGS_current.warmBootDelay = (uint16_t)doc["warmBootDelay"];

  SETTINGS_current.screenDelay == 0 ? 600 : SETTINGS_current.screenDelay;
  SETTINGS_current.coldBootDelay == 0 ? 12000 : SETTINGS_current.coldBootDelay;
  SETTINGS_current.warmBootDelay == 0 ? 1500 : SETTINGS_current.warmBootDelay;

  Serial.print("use screenDelay ");
  Serial.println(SETTINGS_current.screenDelay);
  Serial.println((uint16_t)doc["screenDelay"]);

  Serial.print("use coldBootDelay ");
  Serial.println(SETTINGS_current.coldBootDelay);
  Serial.println((uint16_t)doc["coldBootDelay"]);

  Serial.print("use warmBootDelay ");
  Serial.println(SETTINGS_current.warmBootDelay);
  Serial.println((uint16_t)doc["warmBootDelay"]);

  Serial.print("use extra ");
  Serial.println(SETTINGS_current.useExtraCombo);
  Serial.println((bool)doc["useExtraCombo"]);

  Serial.print("analogThreshold");
  Serial.println(SETTINGS_current.analogThreshold);
  Serial.println((uint16_t)doc["analogThreshold"]);

  
  Serial.print("diagonalThreshold");
  Serial.println(SETTINGS_current.diagonalThreshold);
  Serial.println((uint16_t)doc["diagonalThreshold"]);

  
  Serial.print("throttleThreshold");
  Serial.println(SETTINGS_current.throttleThreshold);
  Serial.println((uint16_t)doc["throttleThreshold"]);
  
  Serial.print("ledBrightness");
  Serial.println(SETTINGS_current.ledBrightness);
  Serial.println((uint16_t)doc["ledBrightness"]);
}