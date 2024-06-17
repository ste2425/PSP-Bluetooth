#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo "Updating index..."

sudo arduino-cli core update-index --log --config-file ../arduino-cli.yaml 

echo "Installing core..."

sudo arduino-cli core install esp32-bluepad32:esp32 --log --config-file ../arduino-cli.yaml 

echo "Installing Libraries..."

sudo arduino-cli lib install ezButton --log --config-file ../arduino-cli.yaml 

echo "Done"