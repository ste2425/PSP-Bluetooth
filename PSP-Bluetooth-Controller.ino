#include <Bluepad32.h>
#include <SPI.h>

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int wiper0writeAddr = B00000000;
const int wiper1writeAddr = B00010000;
const int tconwriteAddr = B01000000;
const int tcon_0off_1on = B11110000;
const int tcon_0on_1off = B00001111;
const int tcon_0off_1off = B00000000;
const int tcon_0on_1on = B11111111;

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

void setup() {

  Serial.begin(9600);
  
  while (!Serial) {
    ;
  }
  
  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // This needs to be behind a reset button later
  BP32.forgetBluetoothKeys();
  
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  // initialize SPI:
  SPI.begin(); 
}

void onConnectedGamepad(GamepadPtr gp) {
  if (myGamepad == nullptr) {
    myGamepad = gp;
    
    Serial.println("CALLBACK: Gamepad is connected");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  if (myGamepad != nullptr && myGamepad == gp) {
    myGamepad = nullptr;
    
    Serial.println("CALLBACK: Gamepad is disconnected");
  }
}


void loop() {
  BP32.update();
  
  if (myGamepad && myGamepad->isConnected()) {  
    int axisX = myGamepad->axisX();
    int axisY = myGamepad->axisY();

    int axisXMapped = map(axisX, -512, 511, 0, 200);
    int axisYMapped = map(axisY, -513, 512, 0, 200);

    
    //Serial.println("Original X");
    //Serial.println(axisX);
    //Serial.println("Mapped X");
    Serial.print(axisY);
    Serial.print(" ");
    Serial.print(axisYMapped);
    Serial.println("");
    //Serial.println("Original Y");
    //Serial.println(axisY);
    //Serial.println("Mapped Y");
    //Serial.println(axisYMapped);

   digitalPotWrite(wiper0writeAddr, axisYMapped);
   digitalPotWrite(wiper1writeAddr, axisXMapped);
  }
  
  //delay(1000);
}

// This function takes care of sending SPI data to the pot.
void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 
}
