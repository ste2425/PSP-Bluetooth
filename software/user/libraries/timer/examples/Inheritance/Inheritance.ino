/*
  Created by Bruno Calou Alves, May, 2015.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"

//Create our class and inherit the Timer class
class FooLed: public Timer {
public:
  FooLed(byte led_pin) {

    this->led_pin = led_pin;
    pinMode(led_pin, OUTPUT);
    led_is_on = false;

    //Set our interval
    this->setInterval(1000);

    //Start the timer
    this->start();
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

  //Overwrites the call method
  void call() {
    blink();
  }

private:
  byte led_pin;
  byte led_is_on;
};


//FooLed object
FooLed led(13);

void setup()
{
}

void loop()
{
  //Update the led
  led.update();
}
