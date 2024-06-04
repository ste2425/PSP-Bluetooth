#!/bin/bash
#UploadSpeed=921600
arduino-cli upload --fqbn esp32-bluepad32:esp32:esp32 --port /dev/ttyUSB0 -i ../bin/software.ino.bin