#!/bin/bash

cd ~/.arduino15/packages/esp32/tools/esptool_py/4.5.1

python ./esptool.py --chip esp32 merge_bin \
  -o ~/code/PSP-Bluetooth/software/bin/merged-firmware.bin \
  --flash_mode dio \
  --flash_freq keep \
  --flash_size 4MB \
  0x1000 ~/code/PSP-Bluetooth/software/bin/software.ino.bootloader.bin \
  0x8000 ~/code/PSP-Bluetooth/software/bin/software.ino.partitions.bin \
  0x10000 ~/code/PSP-Bluetooth/software/bin/software.ino.bin