/*
  Created by Bruno Calou Alves, Apr, 2017.
  Link - https://github.com/brunocalou/Timer
  Read LICENSE for more information.
*/

#include "timer.h"
#include "timerManager.h"

Timer timer1;
Timer timer2;
Timer timer3;
Timer timer4;
Timer timer5;

void sayHello1() {
  Serial.println("Hello from the timer 1");
}

void sayHello2() {
  Serial.println("Hello from the timer 2");
}

void sayHello3() {
  Serial.println("Hello from the timer 3");
}

void sayHello4() {
  Serial.println("Hello from the timer 4");
}

void sayHello5() {
  Serial.println("Hello from the timer 5. I'll say it again...");
}

void setup() {
  Serial.begin(9600);

  //Set the interval in milliseconds
  timer1.setTimeout(1000);
  timer2.setTimeout(2000);
  timer3.setTimeout(3000);
  timer4.setTimeout(4000);
  timer5.setInterval(5000);

  //Set our callback function
  timer1.setCallback(sayHello1);
  timer2.setCallback(sayHello2);
  timer3.setCallback(sayHello3);
  timer4.setCallback(sayHello4);
  timer5.setCallback(sayHello5);

  //Start all the timers
  TimerManager::instance().start();
}

void loop()
{
  //Update all the timers at once
  TimerManager::instance().update();
}
