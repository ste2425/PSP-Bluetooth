# PSP-Consolizer

An Arduino based board to turn a PSP-2000 into a console you can play on your TV using bluetooth based controllers.


## Last update video

<a href="https://youtu.be/4dDin1_Opjc" title="Music Caster Video Demo">
  <p align="center" style="
    position: relative;
  ">
    <img src="./overlay.png" stlye="position: absolute" style="
    position: absolute;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 50px;
    height: 50px;
    top: 50%; ">
    <img width="75%" src="https://img.youtube.com/vi/4dDin1_Opjc/hqdefault.jpg"   alt="Music Caster Video Demo Thumbnail">
  </p>
</a>

## Current functionality
* A 3d printed mount capable of holding the PSP-2000 motherboard and the PSP-Cosolizer board, more info [here](./3D%20models/3dModels.md)
* Full controll over the PSP's face buttons
* Auto power on/off when a controller is connected/disconnected
* Auto tv out (using the PSP's tv out cable) when console turned on.
* Powerfull button mapping system. Old documentation [here](./docs/mappings.md)

## Upcoming functionality
* psp-3000 support (this will be solderless)
* App built to run on the PSP itself to allow for mappings to be built on the fly
* Auto select mappings based on the game being played
* Auto zoom mode for HDMI adapters which have a zoom button
* full documentation
* One click update of PP-Consolizer firmware using a custom built tool (Over the air updates not supported)

## Previous videos

### Controller Mappings
<a href="https://youtu.be/pflKwbyAxQ8" title="Music Caster Video Demo">
  <p align="center" style="
    position: relative;
  ">
    <img src="./overlay.png" stlye="position: absolute" style="
    position: absolute;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 50px;
    height: 50px;
    top: 50%; ">
    <img width="75%" src="https://img.youtube.com/vi/pflKwbyAxQ8/hqdefault.jpg"   alt="Music Caster Video Demo Thumbnail">
  </p>
</a>

## Dev demo PT3
<a href="https://youtu.be/_pwvS-qsQaA" title="Music Caster Video Demo">
  <p align="center" style="
    position: relative;
  ">
    <img src="./overlay.png" stlye="position: absolute" style="
    position: absolute;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 50px;
    height: 50px;
    top: 50%; ">
    <img width="75%" src="https://img.youtube.com/vi/_pwvS-qsQaA/hqdefault.jpg"   alt="Music Caster Video Demo Thumbnail">
  </p>
</a>

## Dev demo PT2
<a href="https://youtu.be/zTRF4sWweqI" title="Music Caster Video Demo">
  <p align="center" style="
    position: relative;
  ">
    <img src="./overlay.png" stlye="position: absolute" style="
    position: absolute;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 50px;
    height: 50px;
    top: 50%; ">
    <img width="75%" src="https://img.youtube.com/vi/zTRF4sWweqI/hqdefault.jpg"   alt="Music Caster Video Demo Thumbnail">
  </p>
</a>

## Dev demo PT1
<a href="https://youtu.be/aO6hr9lEFRw" title="Music Caster Video Demo">
  <p align="center" style="
    position: relative;
  ">
    <img src="./overlay.png" stlye="position: absolute" style="
    position: absolute;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 50px;
    height: 50px;
    top: 50%; ">
    <img width="75%" src="https://img.youtube.com/vi/aO6hr9lEFRw/hqdefault.jpg"   alt="Music Caster Video Demo Thumbnail">
  </p>
</a>

## Libraries
The following libraries are used

* [Bluepad32 - Arduino](https://gitlab.com/ricardoquesada/bluepad32-arduino/-/tree/main)
  * Fantastic library to actually communicate with the controller. Supports multiple architectures, this repository is the Arduino specific implementation.
* [WiFiNINA](https://www.arduino.cc/reference/en/libraries/wifinina/)
  * This is needed to put the Arduino in pass through mode to flash the Bluepad32 firmware to the nina chip.
  * The Bluepad32 library also provides this pass through example sketch to `WIFININA` library **may** not be needed.
