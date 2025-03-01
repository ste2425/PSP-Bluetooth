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
  SETTINGS_current.shutdownDelay = (uint16_t)doc["shutdownDelay"];
  SETTINGS_current.useAlternativeShutdown = (bool)doc["useAlternativeShutdown"];
  SETTINGS_current.batteryWarning = (uint16_t)doc["batteryWarning"];
  SETTINGS_current.dblClickMode = (uint16_t)doc["dblClickMode"];
  SETTINGS_current.lngClickMode = (uint16_t)doc["lngClickMode"];
  SETTINGS_current.clickMode = (uint16_t)doc["clickMode"];

  SETTINGS_current.screenDelay == 0 ? 600 : SETTINGS_current.screenDelay;
  SETTINGS_current.coldBootDelay == 0 ? 12000 : SETTINGS_current.coldBootDelay;
  SETTINGS_current.warmBootDelay == 0 ? 1500 : SETTINGS_current.warmBootDelay;
  SETTINGS_current.shutdownDelay == 0 ? 3000 : SETTINGS_current.shutdownDelay;
  SETTINGS_current.batteryWarning == 0 ? 51 : SETTINGS_current.batteryWarning; //20% of 255

  SETTINGS_current.lngClickMode == 0 ? 2 : SETTINGS_current.lngClickMode; // default to ble
  SETTINGS_current.clickMode == 0 ? 1 : SETTINGS_current.clickMode; // default to controller pair

  // ensure at least one click mode will enable BLE mode as a emegency backup
  if (SETTINGS_current.lngClickMode != 2 && SETTINGS_current.dblClickMode != 2 && SETTINGS_current.clickMode != 2) {
    Serial.print("WARNING: No button mode set to BLE, default long click");
    SETTINGS_current.lngClickMode = 2;
  }

  Serial.print("batteryWarning ");
  Serial.println(SETTINGS_current.batteryWarning);

  Serial.print("use screenDelay ");
  Serial.println(SETTINGS_current.screenDelay);

  Serial.print("use coldBootDelay ");
  Serial.println(SETTINGS_current.coldBootDelay);

  Serial.print("use warmBootDelay ");
  Serial.println(SETTINGS_current.warmBootDelay);

  Serial.print("use extra ");
  Serial.println(SETTINGS_current.useExtraCombo);

  Serial.print("analogThreshold");
  Serial.println(SETTINGS_current.analogThreshold);
  
  Serial.print("diagonalThreshold");
  Serial.println(SETTINGS_current.diagonalThreshold);
  
  Serial.print("throttleThreshold");
  Serial.println(SETTINGS_current.throttleThreshold);
  
  Serial.print("ledBrightness");
  Serial.println(SETTINGS_current.ledBrightness);
  
  Serial.print("shutdownDelay");
  Serial.println(SETTINGS_current.shutdownDelay);
  
  Serial.print("useAlternativeShutdown");
  Serial.println(SETTINGS_current.useAlternativeShutdown);
}