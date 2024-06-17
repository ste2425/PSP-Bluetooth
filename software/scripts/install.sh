#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo "Updating index..."

arduino-cli core update-index --log 

echo "Installing core..."

arduino-cli core install arduino:samd --log 

echo "Installing Libraries..."

arduino-cli lib install ezButton --log

echo "Done"