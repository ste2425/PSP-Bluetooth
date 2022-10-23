#include "timer.h"
#include "timerManager.h"

Timer::Timer() {
	initial_time = 0;
	current_time = 0;
	interval = 0;
	last_interval_time = 0;
	repeat_count = -1;
	is_running = false;
	is_paused = false;
	interval_is_setted = false;
	function_callback = NULL;
	TimerManager::instance().add(this);
}

Timer::~Timer() {
	TimerManager::instance().remove(this);
}

void Timer::start() {
	if(isPaused()) {
		int paused_time = millis() - current_time;
		current_time = millis();
		initial_time += paused_time;
		last_interval_time += paused_time;
	} else {
		reset();
		repeat_count = total_repeat_count;
	}

	is_running = true;
	is_paused = false;
}

void Timer::stop() {
	is_running = false;
	is_paused = false;
}

void Timer::pause() {
	is_running = false;
	is_paused = true;
}

void Timer::reset() {
	initial_time = millis();
	current_time = initial_time;
	last_interval_time = initial_time;
}

bool Timer::isPaused() {
	return is_paused;
}

bool Timer::isStopped() {
	return !is_paused && !is_running;
}

bool Timer::isRunning() {
	return is_running;
}

unsigned long Timer::getElapsedTime() {
	return current_time - initial_time;
}

void Timer::update() {
	if(is_running) {
		current_time = millis();

		if(interval_is_setted) {
			if(current_time - last_interval_time >= interval) {
				if(repeat_count == 0) {
					stop();
					return ;
				} else if(repeat_count > 0) {
					repeat_count -= 1;
				}
				call();
				last_interval_time = current_time;

			}
		}
	}
}

void Timer::setInterval(unsigned long interval, int repeat_count){
	this->interval = interval;
	this->repeat_count = repeat_count;
	total_repeat_count = repeat_count;
	interval_is_setted = true;
}

void Timer::setTimeout(unsigned long timeout){
	setInterval(timeout, 1);
}

void Timer::clearInterval() {
	repeat_count = -1;
}

void Timer::call() {
	if(function_callback != NULL) {
		function_callback();
	}
}
