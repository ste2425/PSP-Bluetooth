#include "timeout.h"

Timeout::Timeout() {
	_running = false;
	_interval = 0;
}

bool Timeout::running() {
	return _running;
}

void Timeout::loop() {
	if (!_running || _interval == 0) {
		return;
	}

  	unsigned long currentMillis = millis();  

	if (currentMillis - _previousMillis >= _interval) {
		_running = false;

		if (_callback != NULL) {
			_callback();
		}
	}
}

void Timeout::setTimeout(int timeout) {
	_interval = timeout;
}

void Timeout::start() {
	_running = true;
	_previousMillis = millis();
}

void Timeout::stop() {
	_running = false;
}