# ESP32 Software

This is the firmware that runs on the ESP32.

# Setup

 To build it the Arduino CLI needs to be installed with the command `arduino-cli` globally available. 

 Once setup in the scripts folder run `install.sh`. This will install all the needed board definitions and cores. It will also install the various libraries needed. Some libraries are to packaged via Arduino and are stored as zip files within this repo.

 # Building

 If you just wish to compile/build the firmware run the script `compile.sh`. This will not merge the bootloader binaries to flash the ESP32.

 # Building && merging

 If you wish to compile/build the firmware and also merge the bootloader binary (The CI does this) run the `build.sh` script.

 We need to do the merging for when uploading to the ESP32 via the web app. *If uploading without the web app use the `upload.sh` script*

 # Building & Uploading

 If you wish to compile and upload the firmware locally run the `upload.sh` script.

------------------------

 ℹ **Default Port** ℹ

 ---------------------

 The scripts are hard coded with the default port the ESP32 appears on for my development machine. If this is different for you, you will need to change it.

 # GitHub Actions

 See the `buildPSPBluetooth.yml` file in the `.github/workflows` folder in the root.

 ## Release binaries for the web app Issue

 See the readme [here](../releaseBinaries/Readme.md)