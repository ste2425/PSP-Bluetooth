/*
  Created by Bruno Calou Alves, May, 2015.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"

//Timer object
Timer timer;

//incomming command
char command = ' ';

void printElapsedTime() {
  Serial.print("Elapsed time = ");
  Serial.print(timer.getElapsedTime());
  Serial.println(" ms");
}

void setup()
{
  Serial.begin(9600);

  //Set the interval in milliseconds we want the led to blink
  timer.setInterval(1000);

  //Set our callback function
  timer.setCallback(printElapsedTime);

  //Start the timer
  timer.start();
}

void loop()
{
  if(Serial.available()) {
    command = Serial.read();

    if(command == 's') {
      timer.start();
      Serial.println("Started");
    } else if(command == 't') {
      timer.stop();
      Serial.println("Stopped");
    } else if(command == 'p') {
      timer.pause();
      Serial.println("Paused");
    } else if(command == 'r') {
      timer.reset();
      Serial.println("Resetted");
    }
  }
  //Update the timer
  timer.update();
}
