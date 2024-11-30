#pragma once

#include "Arduino.h"

class Timeout {
	public:
		Timeout();
		void setCallback(void (*function)(void)){
			this->_callback = function;
		}
		void loop();
		void setTimeout(int timeout);
		void start();
		void stop();
		bool running();

	private:
		unsigned long _previousMillis;
		void (*_callback)(void);
		bool _running;
		int _interval;
};