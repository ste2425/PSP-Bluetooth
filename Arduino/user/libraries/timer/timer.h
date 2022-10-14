/*
	timer.h - The Timer Class works as a simple time counter or
	as a callback caller. It supports callbacks from functions or
	methods, both with void args.

	Link - https://github.com/brunocalou/Timer

	Created by Bruno Calou Alves, May, 2015.
	Read LICENSE for more information.
*/

#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef NULL
#define NULL 0
#endif

class Timer {
public:
	Timer();

	~Timer();

	/*
		Starts the timer
	*/
	void start();

	/*
		Stops and resets the timer
	*/
	void stop();

	/*
		Pauses the timer. It can continue where it paused
		after calling start method
	*/
	void pause();

	/*
		Resets the timer elapsed time. If the timer is running, it will continue to do so
	*/
	void reset();

	/*
		Returns the elapsed time in milliseconds
	*/
	unsigned long getElapsedTime();

	/*
		Sets the interval time. The callback function will
		be called every "interval" ms. The repeat count
		will prevent the callback from being called after
		a number of repetetions (if it's > 0). WARNING: The
		timer will stop after that (if repeat count > 0)
	*/
	void setInterval(unsigned long interval, int repeat_count=-1);

	/*
		Sets the timeout time. The callback function will
		be called just once. WARNING: The timer will stop
		after that
	*/
	void setTimeout(unsigned long timeout);

	/*
		Removes the interval, so the timer can be used as a
		normal timer
	*/
	void clearInterval();

	/*
		Sets the callback function
	*/
	void setCallback(void (*function)(void)){
		this->function_callback = function;
	}

	/*
		Updates the timer. It must be called in a loop in order
		to make it work
	*/
	void update();

	bool isPaused();
	bool isStopped();
	bool isRunning();

protected:
	/*
		Calls the callback function
	*/
	virtual void call();

private:
	unsigned long initial_time;
	unsigned long current_time;

	bool is_running;
	bool is_paused;

	unsigned long interval;
	unsigned long last_interval_time;
	int repeat_count;
	int total_repeat_count;
	bool interval_is_setted;

	void (*function_callback)(void);
};

/*
	TimerForMethods - Class that inherits Timer class and
	overwrites it's call method to support methods as
	a callback
*/
template <class Obj>

class TimerForMethods: public Timer {
public:
	TimerForMethods(Obj *object, void (Obj::*callback)(void)) {
		this->object = object;
		this->method = callback;
	}

	virtual void call() {
		return (object->*method)();
	}

private:
	Obj *object;
	void (Obj::*method)(void);
};
#endif // TIMER_H
