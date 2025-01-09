# Release 2.0.0

Major updates for button mappings and general stability updates

---

⚠ **NOTE** ⚠

This only supports Consolizers.

---

## Features

* Button configuration system expanded to allow a much larger payload. This allows for a greater number of configurations to be saved.
* Added ability for configurations to have a player number bound. Controllser which do not have built in LED's will use this to indicate the active configuration.
* Added ability to reset configurations to the system default.
* Added ability to map the controller right analog stick to PSP buttons. This facilitates a more modern layout for FPS games. (Sensitvity may need to be tweaked in future releases after user feedback)
* Added ability to cycle configurations by pressing `L1` & `R1` & `DPAD Right` instead of clicking the left analock stick.

## Changes

* BLE Mode will now turn itself off after 2.5 minuets of no active connections.
* Controller syncing and BLE mode cannot be enabled at the same time anymore.
* Saving controller configurations saves to a tempory location and is only applied once verified.

## Bug fixes

* Fixed an issue where changing to a new configuration would sometimes not update the connected controllers LED.
* Fixed issues where sometimes saving configurations would cause the browser to lock up and require a reboot.
