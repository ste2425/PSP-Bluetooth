#include "digipot.h"

const int slaveSelectPin = 10;
const int xAddr = B00000000; // 0b00000000;
const int yAddr = B00010000;// 0b00010000;

void write(int address, int value) {
  digitalWrite(slaveSelectPin,LOW);

  SPI.transfer(address);
  SPI.transfer(value);

  digitalWrite(slaveSelectPin,HIGH); 
}

void DIGIPOT_write_x(int value) {
  write(xAddr, value);
}

void DIGIPOT_write_y(int value) {
  write(yAddr, value);  
}

void DIGIPOT_setup() {  
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin(); 
}
