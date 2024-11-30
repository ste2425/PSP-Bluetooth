#include "PSPState.h"

Adafruit_MCP23008 mcpBankOne;
Adafruit_MCP23008 mcpBankTwo;

uint32_t xAxisToSet = 0;
uint32_t yAxisToSet = 0;

uint32_t currentXAxis = 0;
uint32_t currentYAxis = 0;

// GPIO pin for each PSP button
// array index referenced by GPIOPins entries value
uint8_t pspGPIO[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  23,
  17
};

// Indicates which IO expander used for each PSP button
// array index referenced by GPIOPins entries value
// 0/1 MCP 2 ESP
uint8_t mcpBank[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2
};

// Indicates if a PSP button is pressed now
// array index referenced by GPIOPins entries value
bool currentlyPressed[] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false
};

// Indicates if a PSP button is to be pressed in the next tick
// This combined with currentlyPressed allows the GPIO pin to be toggle on state change, so it only happens once, not every tick
// array index referenced by GPIOPins entries value
bool toBePressed[] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false
};

void togglePSPPin(uint8_t pin, int delayTime) {
  uint8_t bank = mcpBank[pin];
  
  if (bank == 0) {
    mcpBankOne.pinMode(pspGPIO[pin], OUTPUT);
    mcpBankOne.digitalWrite(pspGPIO[pin], LOW);
  } else if (bank == 1) {
    mcpBankTwo.pinMode(pspGPIO[pin], OUTPUT);
    mcpBankTwo.digitalWrite(pspGPIO[pin], LOW);
  } else if (bank == 2) {
    pinMode(pspGPIO[pin], OUTPUT);
    digitalWrite(pspGPIO[pin], LOW);
  }
  
  delay(delayTime);
  
  if (bank == 0) {
    mcpBankOne.pinMode(pspGPIO[pin], INPUT);
  } else if (bank == 1) {
    mcpBankTwo.pinMode(pspGPIO[pin], INPUT);
  } else if (bank == 2) {
    pinMode(pspGPIO[pin], INPUT);
  }
}

bool poweredOn = false;

bool PSPstate_poweredOn() {
  return poweredOn;
}

void PSPState_pressScreen() {
  uint8_t pin = pspGPIO[GPIOPins::Display];
  
  mcpBankTwo.pinMode(pin, OUTPUT);
  mcpBankTwo.digitalWrite(pin, LOW);
}

void PSPState_releaseScreen() {
  uint8_t pin = pspGPIO[GPIOPins::Display];

  mcpBankTwo.pinMode(pin, INPUT);
}

void PSPState_togglePower() {
  poweredOn = !poweredOn;
  togglePSPPin(GPIOPins::Power, 800);
}

void PSPState_setup() {  
  pinMode(MCPGND, OUTPUT);
  digitalWrite(MCPGND, LOW);
  
  pinMode(pspGPIO[GPIOPins::Start], INPUT);
  pinMode(pspGPIO[GPIOPins::Select], INPUT);

  pinMode(ANALOGTOGGLE, OUTPUT);
  //digitalWrite(ANALOGTOGGLE, LOW);

  digitalWrite(ANALOGTOGGLE, HIGH);
  
  // give expanders enough time to boot up once ESP pin sent low
  delay(10);
  
  DIGIPOT_write_y(0);
  DIGIPOT_write_x(0);
  
  mcpBankOne.begin();
  mcpBankTwo.begin(1);
}

void PSPState_markAllReleased() {
  for (int i = 0; i < 18; i++) {
    toBePressed[i] = false;
  }

  xAxisToSet = 0;
  yAxisToSet = 0;
}

void PSPState_markButtonAsPressed(uint8_t button) {
  toBePressed[button] = true;
}

void PSPState_markButtonAsReleased(uint8_t button) {
  toBePressed[button] = false;
}


void PSPState_analogX(int value) {
  bool outDeadzone = value > 60 || value < -60;

  if (outDeadzone) {
    xAxisToSet = value;
  }
}

void PSPState_analogY(int value) {
  bool outDeadzone = value > 60 || value < -60;

  if (outDeadzone) {
    yAxisToSet = value;
  }
}

void PSPState_analog(int valueX, int valueY) {  
  PSPState_analogX(valueX);
  PSPState_analogY(valueY);
  
  /*
  if (outDeadzoneX || outDeadzoneY) {
    digitalWrite(ANALOGTOGGLE, HIGH);
    DIGIPOT_write_y(valueY);
    DIGIPOT_write_x(valueX);
  } else {
    digitalWrite(ANALOGTOGGLE, LOW);
  }*/
}

void PSPState_loop() {

  if (xAxisToSet != currentXAxis) {
    currentXAxis = xAxisToSet;
    
    DIGIPOT_write_x(currentXAxis);
  }

  if (yAxisToSet != currentYAxis) {
    currentYAxis = yAxisToSet;
    
    DIGIPOT_write_y(currentYAxis);
  }
  
  for (int i = 0; i < 18; i++) {
    bool pressed = currentlyPressed[i];
    bool toPress = toBePressed[i];
    uint8_t bank = mcpBank[i];

    // if PSP button not currently pressed but needs to be, pres it this tick
    if (toPress && !pressed) {
      currentlyPressed[i] = true;
      
      if (bank == 0) {
        mcpBankOne.pinMode(pspGPIO[i], OUTPUT);
        mcpBankOne.digitalWrite(pspGPIO[i], LOW);
      } else if (bank == 1) {
        mcpBankTwo.pinMode(pspGPIO[i], OUTPUT);
        mcpBankTwo.digitalWrite(pspGPIO[i], LOW);
      } else if (bank == 2) {
        pinMode(pspGPIO[i], OUTPUT);
        digitalWrite(pspGPIO[i], LOW);
      }
    }

    // If PSP button is currently pressed but should not be, release it this tick
    if (!toPress && pressed) {
      currentlyPressed[i] = false;
      
      if (bank == 0) {
        mcpBankOne.pinMode(pspGPIO[i], INPUT);
      } else if (bank == 1) {
        mcpBankTwo.pinMode(pspGPIO[i], INPUT);
      } else if (bank == 2) {
        pinMode(pspGPIO[i], INPUT);
      }
    }  
  }
}
