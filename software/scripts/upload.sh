#!/bin/bash

echo "Compiling"

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BIN_DIR="$SCRIPTS_DIR/../bin"
PROJECT_BIR="$SCRIPTS_DIR/../"
YAML_NAME="../arduino-cli.yaml"

if [ -n "${GITHUB_ACTION}" ]; then
    echo "Using runner yaml"
    YAML_NAME="../arduino-cli-runner.yaml"
fi

echo "   Bin DIR: $BIN_DIR"
echo "   Project DIR: $PROJECT_BIR"

echo "Deleting bin..."
rm -r $BIN_DIR

echo "Compiling..."
arduino-cli compile --port COM9 --upload --log --fqbn esp32-bluepad32:esp32:esp32 --output-dir $BIN_DIR $PROJECT_BIR --config-file $YAML_NAME 

echo "Done"