/*
  Created by Bruno Calou Alves, May, 2015.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"

int led_pin = 13;  //Set our led pin

//Timer object
Timer timer;

//Set our awesome turn on led function
void turnOnLed() {
  digitalWrite(led_pin, HIGH);
}

void setup()
{
  pinMode(led_pin, OUTPUT);

  //The led will turn on after 3 seconds
  timer.setTimeout(3000);

  //Set our callback function
  timer.setCallback(turnOnLed);

  //Start the timer
  timer.start();
}

void loop()
{
  //Update the timer
  timer.update();
}
