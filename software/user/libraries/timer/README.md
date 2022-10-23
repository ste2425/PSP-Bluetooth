# Timer
Arduino timer library. Perfect for removing `delay()` functions from your code and to create asynchronous tasks easily.

It supports callbacks with functions and methods, so you can use it inside your classes

------------------------

## Installation
1. [Download](https://github.com/brunocalou/Timer/archive/master.zip) the latest release from [github](https://github.com/brunocalou/Timer)
2. Unzip and rename the folder to "Timer" (remove the -master)
3. Paste the modified folder on your Library folder (.../Arduino/libraries)
4. Reopen the Arduino IDE

------------------------

## Usage

Let's see the basic usage

```cpp
#include "timer.h"

// Create the timer instance
Timer timer;

void helloCallback() {
  Serial.println("Hello");
}

void setup()
{
  Serial.begin(9600);
  
  // The timer will repeat every 1000 ms
  timer.setInterval(1000); 

  // The function to be called
  timer.setCallback(helloCallback);

  // Start the timer
  timer.start();
}

void loop()
{
  //Update the timer
  timer.update();
}
```

Using the same code above, you can change it to call the helloCallback a limited number of times
```cpp
  // The timer will repeat every 1000 ms and will call the callback only 5 times
  timer.setInterval(1000, 5);
```

If you want to call only once, there is a method for that

```cpp
  // Will call the callback only once
  timer.setTimeout(1000);
```

> Now imagine I have a class. How would I use the Timer if it works only with functions?

Easy, wrap the method inside a function

```cpp
  void callMyMethod() {
    instance.myMethod();
  }
  
  timer.setCallback(callMyMethod);
```

Or, you can use the TimerForMethods class

```cpp
  MyClass myInstance;
  TimerForMethods<MyClass> timer(&myInstance, &MyClass::myMethod);
```

Altough it's more verbose, it's very useful if a timer is beeing used inside a class.

> What if I have a lot of timers? Do I have to keep calling start, update, stop, ..., for every single timer?

Of course not! I'm here to help. Look at this magic

```cpp
#include "timer.h"
#include "timerManager.h"

Timer timer1;
Timer timer2;
Timer timer3;
Timer timer4;
Timer timer5;

... // Write your callbacks

void setup() {
  ... // Write your setup logic
  
  //Start all the timers
  TimerManager::instance().start();
}

void loop() {
  ... // Write your loop logic
  
  //Update all the timers at once
  TimerManager::instance().update();
```

------------------------

## Library Reference

### `Timer` class

*  `Timer()` - Constructor

*  `~Timer()` - Destructor

*  `void start()` - Starts the timer

*  `void stop()` - Stops and resets the timer

*  `void pause()` - Pauses the timer. It can continue where it paused after calling start method

* `void reset()` - Resets the timer elapsed time. If the timer is running, it will continue to do so

*  `unsigned long getElapsedTime()` - Returns the elapsed time in milliseconds

*  `void setInterval(unsigned long interval, int repeat_count=-1)` - Sets the interval time. The callback function will be called every "interval" ms. The repeat count will prevent the callback from being called after a number of repetetions (if it's > 0). WARNING: The timer will stop after that (if repeat count > 0)

*  `void setTimeout(unsigned long timeout)` - Sets the timeout time. The callback function will be called just once. WARNING: The timer will stop after that

*  `void clearInterval()` - Removes the interval, so the timer can be used as a normal timer

*  `void setCallback(void (*function)(void))` - Sets the callback function

*  `void update()` - Updates the timer. It must be called in a loop in order to make it work

*  `bool isPaused()` - Check if the timer is paused

*  `bool isStopped()` - Check if the timer is stopped

*  `bool isRunning()` - Check if the timer is running

*  **protected** `virtual void call()` - Calls the callback function

*  **private** `unsigned long initial_time` - Stores the start time

*  **private** `unsigned long current_time` - Stores the current time

*  **private** `bool is_running` - Stores if the timer is running

*  **private** `bool is_paused` - Stores if the timer is paused

*  **private** `unsigned long interval` - Stores the interval, if setted

*  **private** `unsigned long last_interval_time` - Stores the last time the callback was called

*  **private** `int repeat_count` - Stores the number of times remaining to call the callback function

*  **private** `int total_repeat_count` - Stores the number of times to call the callback function

*  **private** `bool interval_is_setted` - Stores if the interval was setted

*  **private** `void (*function_callback)(void)` - Stores the callback function

### `TimerForMethods` class

*  `TimerForMethods(Obj *object, void (Obj::*callback)(void))` - Constructor

*  `virtual void call()` - Calls the callback method

*  **private** `Obj *object` - Stores the object to be used

*  **private** `void (Obj::*method)(void)` - Stores the method to be called

### `TimerManager` class

*  `static TimerManager& instance()` - Get the TimerManager instance
*  `void update()` - Updates all timers
*  `void start()` - Starts all timers
*  `void stop()` - Stops and resets all timers
*  `void pause()` - Pauses all timers
*  `void reset()` - Resets elapsed time of all timers. If any timer was running, it will continue to do so
*  **private** `void add(Timer *timer)` - Adds a timer to the TimerManager
*  **private** `bool remove(Timer *timer)` - Removes a timer from the TimerManager. Return true if the timer was removed, false otherwise
*  **private** `TimerManager()` - Constructor
*  **private** `TimerManager(TimerManager const &)` - Constructor
*  **private** `void operator=(TimerManager const &)` - Copy operator
*  **private** `class TimerNode` - A node of Timers used to implement a simple linked list
*  **private** `TimerNode *first` - A pointer to the first TimerNode
*  **private** `TimerNode *last` - A pointer to the last TimerNode


### `TimerManager::TimerNode` class

*  `TimerNode *next` - A pointer to the next TimerNode
*  `Timer *value` - A pointer to the Timer instance associated with this node

------------------------

## License
MIT
