# Hardware

This is the source for the Kicad project for the PSP Bluetooth hardware.

# Parts

I use JLCPCB for my assembly and as such the parts used are sourced from their catalog.

## Adding a part

Should i need to add a new part there is a PowerShell script in the root `AddJLCPCBPart.ps1` which will add the part to the various libraries.

# Projects

# Analog

FPC for the analog stick intercept for 2K PSP's. This allows us to inject analog stick values without soldering to the console.

# Buttons Super

FPC for all the buttons intercepts for 2k PSP's combined into a single project. Fabs usually charge extra for all being combined into one.

# Center Buttons

FPC for the center buttons intercept for 2K PSP'script

# ESP

Project for the Mod Board which supports both portable and consolizer

# ESP-C

Project for the Mod Board which only support consolizers. The analog multiplexer for the analog sticks are missing here.

# Right

FPC for just the right buttons for 2K PSP's