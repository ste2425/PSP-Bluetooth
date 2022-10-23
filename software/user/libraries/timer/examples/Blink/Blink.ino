/*
  Created by Bruno Calou Alves, May, 2015.
  Read LICENSE for more information.
*/

#include "timer.h"

int led_pin = 13;  //Set our led pin
bool led_is_on = false;  //Holds if the led is on

//Timer object
Timer timer;

//Set our awesome blink led function
void blinkLed() {
  if(led_is_on) {
    digitalWrite(led_pin, HIGH);
  } else {
    digitalWrite(led_pin, LOW);
  }

  //Change our led state
  led_is_on = !led_is_on;
}

void setup()
{
  pinMode(led_pin, OUTPUT);

  //Set the interval in milliseconds we want the led to blink
  timer.setInterval(1000);

  //Set our callback function
  timer.setCallback(blinkLed);

  //Start the timer
  timer.start();
}

void loop()
{
  //Update the timer
  timer.update();
}
