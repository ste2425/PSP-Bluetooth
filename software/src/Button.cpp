#include "Button.h"

OneButton button;

void doubleClick()
{
  LED_on();
  Serial.println("x2");

} // doubleClick

void click() {
  LED_off();
  LED_fadeSlow();
  Serial.println("Click");
}

void longClick() {
  LED_off();
  Serial.println("Long click");
}

void BUTTON_loop() {
  button.tick();
}

void BUTTON_setup() {
  button.attachDoubleClick(doubleClick);
  button.attachClick(click);
  button.attachLongPressStart(longClick);
  
  button.setup(BUTTON_PIN, INPUT_PULLUP, true);
}
