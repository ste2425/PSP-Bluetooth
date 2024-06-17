#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo "Updating index..."

arduino-cli core update-index --log --additional-urls "https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"

echo "Installing core..."

arduino-cli core install esp32-bluepad32:esp32 --log

echo "Installing Libraries..."

arduino-cli lib install ezButton --log

echo "Done"