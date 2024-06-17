#!/bin/bash

echo "Compiling"

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BIN_DIR="$SCRIPTS_DIR/../bin"
PROJECT_BIR="$SCRIPTS_DIR/../"

echo "   Bin DIR: $BIN_DIR"
echo "   Project DIR: $PROJECT_BIR"

echo "Deleting bin..."
rm -r $BIN_DIR

echo "Compiling..."
arduino-cli compile --log --fqbn esp32-bluepad32:esp32:esp32 --output-dir $BIN_DIR $PROJECT_BIR --config-file ../arduino-cli.yaml 

echo "Done"