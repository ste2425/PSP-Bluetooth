/*
  Created by Bruno Calou Alves, May, 2015.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"

//Create our class (FooLed, got it ?)
class FooLed {
public:
  FooLed(byte led_pin):

  // Set the blink method as the callback on the timer
  timer(this, &FooLed::blink) {

    this->led_pin = led_pin;
    pinMode(led_pin, OUTPUT);
    led_is_on = false;

    //Set our interval
    timer.setInterval(1000);

    //Start the timer
    timer.start();
  }

  //Our awesome blink method
  void blink() {
    if(led_is_on) {
        digitalWrite(led_pin, HIGH);
      } else {
        digitalWrite(led_pin, LOW);
      }

      //Change our led state
      led_is_on = !led_is_on;
  }

  void updateLed() {
    timer.update();
  }

private:
  byte led_pin;
  byte led_is_on;
  TimerForMethods<FooLed> timer;
};


//FooLed object
FooLed led(13);

void setup()
{

}

void loop()
{
  //Update the led
  led.updateLed();
}
