#include "pot.h"

const int slaveSelectPin = 10;
const int xAddr = B00000000; // 0b00000000;
const int yAddr = B00010000;// 0b00010000;

int currentX = 0;
int currentY = 0;

void POT_write(int address, int value) {
  digitalWrite(slaveSelectPin,LOW);

  SPI.transfer(address);
  SPI.transfer(value);

  digitalWrite(slaveSelectPin,HIGH); 
}

void POT_write_X(int value) {
  POT_write(xAddr, value);
}

void POT_write_Y(int value) {
  POT_write(yAddr, value);  
}

void POT_set(int x, int y) {
  int xToSet = constrain(x, 0, 255);
  int yToSet = constrain(y, 0, 255);
  
  if (xToSet != currentX) {
    POT_write(yAddr, xToSet);
    currentX = xToSet;
  }

  if (yToSet != currentY) {
    POT_write(xAddr, yToSet);  
    currentY = yToSet;
  }
}

uint8_t POT_map_controller(int controllerVal) {  
  /*
   * Controllers report range of -511 - 512
   * bring to 0 - 1023 then map to Digital pots acceptable range.
   * Digi pot accepts 0 - 257 however
   * for some reason PSP reports full range from a value of 50 - 200.
   */
   
  return map(controllerVal + 511, 0, 1023, 50, 200);
}

void POT_Centre() {  
  POT_set(127, 127);
}

void POT_setup() {  
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin(); 

  POT_Centre();
}
