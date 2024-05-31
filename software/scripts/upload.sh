#!/bin/bash

arduino-cli upload --fqbn esp32-bluepad32:esp32:esp32 --port /dev/ttyUSB0 -i ../bin/src.ino.bin