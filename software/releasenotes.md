# Release 2.3.2 - PRE RELEASE

## Features
 N/A
## Changes

## Saving mappings doesnt reset active mapping
When saving mappings via the web editor the currently active mapping will remain the active mapping.

However this is based on mapping number. Imagine you have three configured mappings, with the last one active.
If a mapping is deleted so there is now only 2 active mappings. In this situation the currently active mapping will still be reset back to mapping one.

## Bug fixes

## Saved button mappings not applying correctly

When saving button mappings via the Web UI they were not saving correctly when a controller is connected.

Now they should update correctly. Note: *The active mappings resets to the first mapping*.