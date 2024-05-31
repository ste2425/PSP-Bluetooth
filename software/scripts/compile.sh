#!/bin/bash

echo "Compiling..."
echo "Using extra arguments: $@"

rm -r ../bin

arduino-cli compile --log --fqbn esp32-bluepad32:esp32:esp32 --output-dir ../bin ../ "$@" 