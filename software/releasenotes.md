# Release 2.3.1-RC4

**Pre-Release** This release modifies the warm boot delay to `5` seconds from `2.5` seconds and adds ability to choose Mode Button purpose.

## Features
 N/A
## Changes

### boot delay
The default warm boot time was 2.5 seconds. However some people report that this is not enough time to ensure the PSP is properly booted before enabling TV out.

As such the default time has been extended to 5 seconds.

### Mode Button
Each mode `Click`, `Long Click` and `Double Click` can be configured to either `Toggle new controller connections`, `Toggle BLE mode` or `Toggle PSP power`. This is done via the settings menu.

**⚠ IMPORTANT NOTE ⚠** This only applies to users who have not saved settings on the `Settings Page`. Otherwise the saved settings will be applied.

## Bug fixes

N/A