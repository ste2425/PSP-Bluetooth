# PSP-Bluetooth

--- 

ℹ️ **Name Change**  This project was formerly called the `PSP-Consolizer`. The project has since been renamed to `PSP-Bluetooth` as the term `Consolizer` usually makes people think of video output which this project never worked with. 

If you wish to see the legacy `PSP-Consolizer` project it is accessible on the `legacy` branch [here](https://github.com/ste2425/PSP-Bluetooth/tree/legacy)

---

This project aims to add Bluetooth controller support to the PSP 1k, 2k and 3k models using an ESP32.

---

It aims to solve to problems:

- [x] Using the PSP as a consolizer
- [ ] Using the PSP as a dockable console (like the Nintendo Switch)

## Progress so far
* Fully functional hardware that replaces the UMD drive
* Partially functional hardware for portable version
* Fully functional consolizer implementation that works for both the 2K and 3K consoles
* Fully functional firmware that offer configurable controller mappings and OTA updates
* Single click programming via a Web app for Serial and OTA programming. No extra tools needed, it downloads the binaries and does everything
* Partially functional controller mappings via the web app.

## Web app

The web app can be found here [PSP-Bluetooth](https://ste2425.github.io/PSP-Bluetooth/).

## Repository Structure

### /software

This contains the firmware for the ESP32. It is built using the Arduino core and maanaged via the Arduino CLI. To build it the cli needs to be installed with the command `arduino-cli` globally available. 

See [it's readme](./software/Readme.md) for more info.

### /installer

This contains the software for the Web app. It as an Angular application built using Angular Material and deployed to GitHub pages via GitHub Actions.

See [it's readme](./installer/Readme.md) for more info.

### /Hardware

This contians all the hardware files for custom FPC's and the mod board itself.

See [it's readme](./hardware/Readme.md) for more info.