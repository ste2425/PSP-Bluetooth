/*
  Created by Bruno Calou Alves, May, 2015.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"

//Timer object
Timer timer;

int counter = 5;

void countdown() {
  Serial.println(counter);
  if(counter > 0) {
    counter -= 1;
  }

}
void setup()
{
  Serial.begin(9600);

  //Set the interval in milliseconds
  //and the number of repetitions
  timer.setInterval(1000, 6);

  //Set our callback function
  timer.setCallback(countdown);

  //Start the timer
  timer.start();
}

void loop()
{
  //Update the timer
  timer.update();
}
