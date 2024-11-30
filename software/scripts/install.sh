#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
YAML_NAME="../arduino-cli.yaml"

if [ -n "${GITHUB_ACTION}" ]; then
    echo "Using runner yaml"
    YAML_NAME="../arduino-cli-runner.yaml"
fi

echo "Updating index..."

arduino-cli core update-index --log --config-file $YAML_NAME

echo "Installing core..."

arduino-cli core install esp32-bluepad32:esp32 --log --config-file $YAML_NAME

echo "Installing Libraries..."

arduino-cli lib install ezButton --log --config-file $YAML_NAME
arduino-cli lib install OneButton --log --config-file $YAML_NAME
arduino-cli lib install ArduinoJson --log --config-file $YAML_NAME
arduino-cli lib install "Adafruit MCP23008 library" --log --config-file $YAML_NAME
arduino-cli lib install --zip-path ../libraries/MCP_POT-0.2.2.zip --log --config-file $YAML_NAME

echo "Done"