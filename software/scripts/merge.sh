#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
BIN_DIR="$SCRIPTS_DIR/../bin"
PROJECT_BIR="$SCRIPTS_DIR/../"
ESP_DIR="~/.arduino15/packages/esp32/tools/esptool_py/4.5.1"

if [ -n "${GITHUB_ACTION}" ]; then
  echo "Using DIR"
  ESP_DIR="/home/runner/.arduino15/packages/esp32/tools/esptool_py/4.5.1"
fi

echo "Merging"

echo "   Bin DIR: $BIN_DIR"
echo "   Project DIR: $PROJECT_BIR"

# This feels hacky but cant think of a better way
cd $ESP_DIR

# wonder if esptool is packaged? Maybe i should use that
python3 ./esptool.py --chip esp32 merge_bin \
  -o $BIN_DIR/pspBluetooth.bin \
  --flash_mode dio \
  --flash_freq keep \
  --flash_size 4MB \
  0x1000 $BIN_DIR/software.ino.bootloader.bin \
  0x8000 $BIN_DIR/software.ino.partitions.bin \
  0x10000 $BIN_DIR/software.ino.bin

  echo "Done"