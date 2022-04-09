# PSP-Bluetooth-Controller
An Arduino based addon board to give Bluetooth controller support to a PSP-2000

The goal is to replace the UMD drive with a board that will sit in that location. 
It will allow a Bluetooth controller to control the PSP, but also keep the functionality of the original buttons.

## Libraries
The following libraries are used

* [Bluepad32 - Arduino](https://gitlab.com/ricardoquesada/bluepad32-arduino/-/tree/main)
  * Fantastic library to actually communicate with the controller. Supports multiple architectures, this repository is the Arduino specific implementation.
* [WiFiNINA](https://www.arduino.cc/reference/en/libraries/wifinina/)
  * This is needed to put the Arduino in pass through mode to flash the Bluepad32 firmware to the nina chip.
  * The Bluepad32 library also provides this pass through example sketch to `WIFININA` library **may** not be needed.

## How does it work?
Button presses from the controller are mapped to Arduino pins which connect to the various FFC connectors on the PSP.
Button presses are recognised on the PSP's side by sending the line low. So when the Arduino wants to trigger a button press it marks the pin as Output 
and writes LOW to it. To release the button the Arduino marks the pin as an Input.
