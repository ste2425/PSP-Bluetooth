#include <Bluepad32.h>
#include <SPI.h>

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

// Controller masks ---------------------------------------------
// These are coppied from Gamepad.h from Bluepad32 however they are declared against a Gamepad instance
// That or my lack of C++ knowledge prevents me from accessing them without a gamepad instance.
enum {
  DPAD_UP = 1 << 0,
  DPAD_DOWN = 1 << 1,
  DPAD_RIGHT = 1 << 2,
  DPAD_LEFT = 1 << 3,
};

enum {
  BUTTON_A = 1 << 0,
  BUTTON_B = 1 << 1,
  BUTTON_X = 1 << 2,
  BUTTON_Y = 1 << 3,
  BUTTON_SHOULDER_L = 1 << 4,
  BUTTON_SHOULDER_R = 1 << 5,
  BUTTON_TRIGGER_L = 1 << 6,
  BUTTON_TRIGGER_R = 1 << 7,
  BUTTON_THUMB_L = 1 << 8,
  BUTTON_THUMB_R = 1 << 9,
};

enum {
  MISC_BUTTON_SYSTEM = 1 << 0,  // AKA: PS, Xbox, etc.
  MISC_BUTTON_BACK = 1 << 1,    // AKA: Select, Share, -
  MISC_BUTTON_HOME = 1 << 2,    // AKA: Start, Options, +
};

// ---------------------------------------------------------------

typedef enum {
  TYPE_DPAD = 0,
  TYPE_BUTTONS = 1,
  TYPE_SYSTEM = 2
} controllerType;

typedef struct {
  const byte pin;
  const int controllerMask;
  bool pressed;
  controllerType type;
} pinMap;

pinMap mappedPins[] = {
  // Bottom buttons
  { 14, MISC_BUTTON_SYSTEM, false, TYPE_SYSTEM }, // home - D15 - Pin 20
  { 17, MISC_BUTTON_HOME, false, TYPE_SYSTEM }, // start - D14 - Pin 19
  { 16, MISC_BUTTON_BACK, false, TYPE_SYSTEM }, // select - D13 - Pin 16
  { 15, BUTTON_THUMB_R, false, TYPE_BUTTONS },  // screen - D12 - Pin 15

  // Left hand buttons
  { 19, BUTTON_SHOULDER_L, false, TYPE_BUTTONS  }, // Left Shoulder Button - D11 - pin 14
  { 9, DPAD_UP, false, TYPE_DPAD  }, // up - D10 - pin 13
  { 7, DPAD_DOWN, false, TYPE_DPAD  }, // down - D9 - pin 12
  { 18, DPAD_RIGHT, false, TYPE_DPAD  }, // right - D8 - Pin 11
  { 8, DPAD_LEFT, false, TYPE_DPAD  }, // left - D7 - Pin 10
  
  // Right hand buttons
  { 5, BUTTON_SHOULDER_R, false, TYPE_BUTTONS }, // Right shoulder Button - D6 - Pin 9
  { 2, BUTTON_A, false, TYPE_BUTTONS  }, // Cross - D5 - pin 8
  { 3, BUTTON_B, false, TYPE_BUTTONS  }, // Circle - D4 - Pin 7
  { 6, BUTTON_X, false, TYPE_BUTTONS  }, // Square - D3 - Pin 6
  { 4, BUTTON_Y, false, TYPE_BUTTONS  } // Triangle - D2 - Pin 5
};
const byte mappedPinsSize = sizeof(mappedPins) / sizeof(pinMap);

const int slaveSelectPin = 10;
const int wiper0writeAddr = B00000000; // 0b00000000;
const int wiper1writeAddr = B00010000;// 0b00010000;

int currentX = 0;
int currentY = 0;

byte PSPPoweredOn = 0;

void togglePower(){
  pressPin(21);
  delay(500);
  pinMode(21, INPUT);
}

void toggleScreen(){
  pressPin(15);
  delay(5000);
  pinMode(15, INPUT);
}

void powerOn() {
  if (PSPPoweredOn == 0){
    togglePower();
    PSPPoweredOn = 1;
    delay(1000);
    toggleScreen();
  }
}

void powerOff() {
  if (PSPPoweredOn == 1){
    togglePower();
    PSPPoweredOn = 0;
  }
}

/*
 * --------------------------------
 * Analog stick related methods
 * --------------------------------
 */

void setPot(int x, int y) {
  int xToSet = constrain(x, 0, 255);
  int yToSet = constrain(y, 0, 255);
  
  if (xToSet != currentX) {
    digitalPotWrite(wiper1writeAddr, xToSet);
    currentX = xToSet;
  }

  if (yToSet != currentY) {
    digitalPotWrite(wiper0writeAddr, yToSet);
    currentY = yToSet;
  }
}

// Should really use a lib for this
void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 
}

void updateLeftStick(GamepadPtr gamepad) {
    int axisX = gamepad->axisX() + 511;
    int axisY = gamepad->axisY() + 511;

    // For some reason PSP reports full range from a value of 50 - 200.
    int axisXMapped = map(axisX, 0, 1023, 50, 200);
    int axisYMapped = map(axisY, 0, 1023, 50, 200);

    setPot(axisXMapped, axisYMapped);
}

//---------------------------------------

/*
 * --------------------------------------
 * Bluepad32 callbacks
 * --------------------------------------
 */
void onConnectedGamepad(GamepadPtr gp) {
  if (myGamepad == nullptr) {
    myGamepad = gp;
    
    Serial.println("CALLBACK: Gamepad is connected");

    powerOn();
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  if (myGamepad != nullptr && myGamepad == gp) {
    myGamepad = nullptr;
    
    Serial.println("CALLBACK: Gamepad is disconnected");

    powerOff();
  }
}

//------------------------------------------

/*
 * -----------------------------------------
 * Button handling methods
 * -----------------------------------------
 */
void releaseAllPins() {
  for(byte i = 0; i < mappedPinsSize; i++){
    mappedPins[i].pressed = false;
    releasePin(mappedPins[i].pin);
  }
}

void releasePin(byte pin) {
  pinMode(pin, INPUT);
}

void pressPin(byte pin) { 
    Serial.println(pin);     
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void updateButtons(GamepadPtr gamepad) {
  for (byte i = 0; i < mappedPinsSize; i++){
    pinMap *mapping = &mappedPins[i];

    switch (mapping->type) {
      case TYPE_DPAD:
        if (gamepad->dpad() & mapping->controllerMask) {
          if (!mapping->pressed) {
            mapping->pressed = true;
            pressPin(mapping->pin);
          }
        } else {
          if (mapping->pressed) {
            mapping->pressed = false;
            releasePin(mapping->pin);
          }
        }
      break;
      case TYPE_BUTTONS:
        if (gamepad->buttons() & mapping->controllerMask) {
          if (!mapping->pressed) {
            mapping->pressed = true;
            pressPin(mapping->pin);
          }
        } else {          
          if (mapping->pressed) {
            mapping->pressed = false;
            releasePin(mapping->pin);
          }
        }
      break;
      case TYPE_SYSTEM:
        if (gamepad->miscButtons() & mapping->controllerMask) {
          if (!mapping->pressed) {
            mapping->pressed = true;
            pressPin(mapping->pin);
          }
        } else {          
          if (mapping->pressed) {
            mapping->pressed = false;
            releasePin(mapping->pin);
          }
        }
      break; 
    }
  }
}

// -------------------------------------------------

/*
 * ----------------------------------------
 * Arduino lifecycle methods
 * ----------------------------------------
 */

void setup() {
  Serial.begin(9600);
    
  String fv = BP32.firmwareVersion();
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  
  Serial.println("Setup");
  Serial.println(fv);

  // This needs to be behind a reset button later
  BP32.forgetBluetoothKeys();

  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin(); 
  
  setPot(127, 127);
  
  Serial.println("Setup Complete");
}

void loop() {  
  BP32.update();
  
  if (myGamepad && myGamepad->isConnected()) {  
    updateButtons(myGamepad);
    updateLeftStick(myGamepad);

    if (myGamepad->thumbL())
      togglePower();
          
  } else {
    releaseAllPins();
  } 
}

//--------------------------------------
