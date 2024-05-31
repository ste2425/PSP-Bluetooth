# Setup Arduino Cli

* sudo apt-get update
sudo apt-get install python3-pip
pip3 install pyserial

arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 .
Traceback (most recent call last):
  File "/home/stephen/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py", line 31, in <module>
    import esptool
  File "/home/stephen/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool/__init__.py", line 41, in <module>
    from esptool.cmds import (
  File "/home/stephen/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool/cmds.py", line 14, in <module>
    from .bin_image import ELFFile, ImageSegment, LoadFirmwareImage
  File "/home/stephen/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool/bin_image.py", line 14, in <module>
    from .loader import ESPLoader
  File "/home/stephen/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool/loader.py", line 30, in <module>
    import serial
ModuleNotFoundError: No module named 'serial'

Used platform         Version Path
esp32-bluepad32:esp32 4.0.4   /home/stephen/.arduino15/packages/esp32-bluepad32/hardware/esp32/4.0.4
Error during build: exit status 1

arduino-cli compile --fqbn esp32-bluepad32:esp32:esp32 .