<h1 align="center"> PSP Consolizer User Guide </h1>

<p align="center">
    Built by Stephen Cooper
</p>

<p align="center">
    <a href="https://github.com/ste2425/PSP-Consolizer">https://github.com/ste2425/PSP-Consolizer</a>
</p>

<p align="center">
    <img width="80px" src="../logo.png" />
<p>

<p align="center">
This is the user guide for the PSP Consolizer. It contains a brief run down of its functionality.
</p>

<style>
    img[alt*=".p0"] { padding: 0px; }
    img[alt*=".p1"] { padding: 5px; }
    img[alt*=".p2"] { padding: 10px; }
    img[alt*=".pl0"] { padding-left: 0px; }
    img[alt*=".pl1"] { padding-left: 5px; }
    img[alt*=".pl2"] { padding-left: 10px; }

    img[alt*=".large"] {
        max-width: 500px;
    }
    img[alt*=".medium"] {
        max-width: 350px;
    }
    img[alt*=".small"] {
        max-width: 150px;
    }

    img[alt*=".right"] {
        float: right;
    }
    img[alt*=".left"] {
        float: left;
    }
    img[alt*=".center"] {
        display: block;
        margin: 0 auto;
    }

    table {
        width: 100%;
        margin: 20px;
    }

    .clearfix:after {
        content: '';
        display: block;
        clear: both;
    }
</style>

<div class="page"/>

# First power up

The first time the Consolizer powers up you will need to sync a controller.

# Controller syncing

This is done by pressing the sync button on the Consolizer. This will cause the Consolizer to forget about any already synced controllers.

Whilst waiting for connections the power LED will flash.

You will have 9 seconds to connect a controller before it stops listening for connections.

# Powering on / off the Consolizer

The Consolizer will automatically power on and off the PSP when a controller connects/disconnects.

The power LED will flash (faster than when syncing a controller) during the boot up process.

# Changing buttons mappings

There are three button mappings which can be cycled in order. The controllers built in RGB or player indicator LED will be used to show the current mapping (if the controller has one). Controllers which have neither, XBOX One S, will vibrate when a mapping is changed. You will have to guess which mapping it is.

To change the mappings press in the right analog stick and press up on the DPAD. (Wii controllers cannot click their analog stick, so they cannot change mappings. This may be resolved in the future).

| Mapping | LED Colour | Player indicator number |                                                                                                |
| ------- | ---------- | ----------------------- | ---------------------------------------------------------------------------------------------- |
| Default | Blue       | 1                       | The default mapping - All PSP buttons mapped to their equivalent controller buttons            |
| FPS     | Red        | 2                       | Built for FPS games. The right analog stick is mapped to the X, O, Square and Triangle buttons |
| PSX     | Green      | 3                       | Built for PS1 games. R2 and L2 are mapped to the PSP's analog stick                            |

<div class="page"/>

# Changing LED brightness.

Some controllers have a built in LED (Sony DualShock 3/4 DualSense 5 and maybe others). The brightness of this LED can be cycled by clicking in the right analog stick and pressing down on the DPAD.

It will cycle through preset brightnesses.

If the LED is set to be turned off, changing buttons mappings or turning the console on will enable the LED at full brightness for 3 seconds. It will then turn off again.

<div class="page"/>

# FYI

The Consolizer does not actually know if the PSP is on or not. It just keeps track of when it toggle power. If you turn the Arduino off and on whilst the PSP is on it will think the PSP is off when it is actually on.

--

Turning off the consolizer is done by disconnecting controllers. That depends on the controller itself some turn off quick Xbox ONE S, some take a while DS4. There will be a future update to make this process quicker and the same for all controllers. However that requires an update in a library we use so have to wait until that is released.

<div class="page"/>

# End of document
