/*
 * This example checks if the firmware loaded on the NINA module
 * is updated.
 *
 * Supported on boards with NINA W10x like:
 *  - Arduino MKR WiFi 1010,
 *  - UNO WiFi Rev.2,
 *  - Nano RP2040 Connect,
 *  - Nano 33 IoT,
 *  - etc.
 *
 * Created 17 October 2018 by Riccardo Rosario Rizzo
 * This code is in the public domain.
 */
#include <Bluepad32.h>

void setup() {
  // Initialize serial
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect.
  }

  // Print a welcome message
  Serial.println("Bluepdad32 firmware check.");
  Serial.println();

  // Print firmware version on the module
  String fv = BP32.firmwareVersion();
  String latestFv;
  Serial.print("Firmware version installed: ");
  Serial.println(fv);
}

void loop() {
  // do nothing
}
