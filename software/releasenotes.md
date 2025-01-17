# Release 2.1.0-RC1

This pre-releases fixes issues around OTA updates

## Features

N/A

## Changes

N/A

## Bug fixes

* Applying OTA Updates whilst the PSP is on would get the consolizer confused about the state of the PSP. It would think the PSP is turned on when it is turned off and vice versa.

Now any connected controllers are disconnected and the PSP itself turned off, if it was turned on, before the update is applied.
