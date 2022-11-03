<h1 align="center"> PSP Consolizer Build Guide </h1>

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
This is a personal project <em>NOT</em> a commercial product. As such do your own due diligence to be sure this will not damage your PSP or Arduino. 
</p>

<p align="center">
Please don't just take my word for it that it actually. Check everything yourself before spending any money.
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

Building a PSP Consolizer is split into three steps

- [1.1 Having the PCB manufactured](#11---having-the-pcb-manufactured)
  - [1.2 Having the PCB Assembled](#12---having-the-pcb-assembled)
  - [1.3 PCB Parts Layout](#13---pcb-parts-layout)
- [2 Assembling the PCB](#2---assembling-the-pcb)
- [3 Programming the Arduino](#3---programming-the-arduino)
- [4 Connecting the Arduino and PSP](#4---connecting-the-arduino-and-psp)
  - [4.1 Soldering the Analog Stick](#41---soldering-the-analog-stick)

The last step (Arduino programming) can be done at any point, before or after it is installed.

The files included within this release package contain everything you need to complete this project. You do not need any extra software to program the Arduino. Everything is bundled within the installer (thats why its quite large!).

---

⚠ **Important** There is fine soldering involved with the PSP-2000 which is non reversible. This project currently does not support the PSP-3000. Please review the entire process to see if your up to the challenge before spending any money.

---

<div class="page"/>

# 1.1 - Having the PCB manufactured

## Board configuration

I chose [JLCPCB](https://jlcpcb.com/) to manufacture my boards as they were recommended to me. I'm pleased with that have done so far.

I also have JLCPCB fit the difficult to solder surface mounted parts so only the through hole parts remain.

---

ℹ Files have been generated for JLCPCB to manufacture and fit the surface mount parts, however any manufacturer can be used if desired.

---

1. Firstly [create an account then Log in](https://jlcpcb.com/).

2. On the home page begin the process by clicking the **Order Now** link in the top right.  
   ![.right.small.p2](./guide%20images/one.png)  
   Click the **Add Gerber** link and select **PSP-Consolizer-v1.0.zip** from the downloaded release artifacts.  
   &nbsp;  
   Once it loads you should see a preview of the board like below.

&nbsp;

![.center.medium.p2](./guide%20images/two.png)

3. Next scroll down and ensure the various settings match below  
   &nbsp;  
   **Note about certain Settings** Changing some settings such as colours may increase the predicted cost. JLCPCB works by batching orders onto single boards. If you choose an unpopular colour it will take longer for enough orders in that colour to fill a aboard resulting in a greater cost.  
   &nbsp;

<div class="page"/>

| name                    | value                                   |
| ----------------------- | --------------------------------------- |
| Base Material           | FR-4                                    |
| Layers                  | 2                                       |
| Dimensions              | 91.44mm \* 71.12                        |
| PCB Qty                 | 5                                       |
| Product Type            | Industrial/Consumer electronics         |
| Different Design        | 1                                       |
| Delivery Format         | Single PCB                              |
| PCB Thickness           | 1.6                                     |
| PCB Color               | **Pick which you like**                 |
| Silkscreen              | **Will change depending on PCB colour** |
| Outer Copper Weight     | 1 oz                                    |
| Surface Finish          | HASL (with lead)                        |
| Gold Fingers            | No                                      |
| Confirm Production File | No                                      |
| Flying Probe Test       | Fully Test                              |
| Castellated Holes       | No                                      |
| Remove Order Number     | No                                      |

4. Enable PCB Assembly by clicking the toggle.

![.center.large](./guide%20images/three.png)
&nbsp;

<div class="page"/>

# 1.2 - Having the PCB Assembled

1. Select **Assemble top side** and verify the settings match below.

<div align="center">

| name                    | value           |
| ----------------------- | --------------- |
| PCBA Type               | Economic        |
| Assembly Side           | Top Side        |
| PCBA Qty                | 5               |
| Tooling holes           | Added by JLCPCB |
| Confirm Parts Placement | No              |

</div>

2. Click **Confirm**.

3. Next you need to upload the BOM and CPL files.  
   &nbsp;  
   These tell JLCPCB the parts to assemble (Bill Of Materials) and where to place those parts (Component Placement List)  
   &nbsp;  
   For the BOM select **psp-bluetooth_bom_jlc.csv** downloaded from the release artifacts.  
   &nbsp;  
   For the CPL select **psp-bluetooth_cpl_jlc.csv**

4. Select from the dropdown what the board is to be used for, i just selected **DIY** for both options.

5. Click **next** and you will be presented with a page listing the parts that will be used. If it looks like below click **Next**.

![.center.large](./guide%20images/four.png)

<div class="page"/>

# 1.3 - PCB parts layout

Now you will be given a preview of the board layout with the parts on it.

I don't know why but for me when it first loads the parts to assemble will appear in the wrong place.

![.center.medium](./guide%20images/five.png)

To fix this click the **Align** button in the tool bar.

![.center.medium](./guide%20images/six.png)

---

ℹ Is is possible to zoom in the preview using the middle mouse wheel and move about by holding the right mouse button

---

This should result in a preview which looks like this.

![.center.medium](./guide%20images/seven.png)

Once done click **Save to Cart** from there you can go through the purchasing process.

![.center.medium](./guide%20images/goojob.jpg)

<div class="page"/>

# 2 - Assembling the PCB

Next is the job of assembling the board. The following components will be needed.

- Arduino Nano 33 iot
- 2x 10pin 0.5mm pitch FFC cables Type A (contacts on both sides)
- 1x 14pin 0.5mm pitch FFC cable Type A (contacts on both sides)
- MCP 4251 digital potentiometer 14 pin DIP package
- LED colour of choice (See LED below)
  - Suitable resistor for LED
- Mounting hardware (see Mounting Arduino & MCP4251 below)
- Jumpers and pins (see Jumpers below)

## Mounting Arduino & MCP4251

The Arduino and MCP4251 can be mounted permanently or be removable. The MCP4251 can be soldered directly to the PCB or using a DIP socket.

The Arduino can be mounted directly to the PCB using its castellated pins or via Male/Female header pins. The first option gives it the lowest profile but header pins allows you to use the Arduino for another project in the future.

## Jumpers

The Arduino is powered via its **vin** pin. Iv'e heard conflicting reports that powering it via this pin and USB at the same time (when programming) will cause damage. Others also say if you only USB it will back feed the **vin** pin, in our case powering up the PSP.

As a result there is a jumper **PWR TGL 1** and solder jumper **JP1**. If you know what your doing, unlike me, you can permanently solder **JP1**. You can also use a jumper pin instead (like me) and simply remove the jumper when programming the Arduino, disconnecting the **vin** pin from the Arduino.

## LED

The LED is powered directly from the Arduino, using one of its serial pins. As a result choose an LED and resistor which do not overpower the Arduino pin.

As it's powered from a Serial pin Iv'e read conflicting reports that having a load on those pins can cause programming errors.

As a result there is a jumper **PROG TGL 1** and a solder jumper **JP2**. If you are confident it will not cause issues (it didn't in my case) you can permanently solder it. Otherwise, like the USB/**vin** comment you can remove the jumper when programming.

<div class="page"/>

# 2.1 - Fitting the LED

Begin by fitting the resistor so it is flush with the PCB and bend its legs outwards to hold it in place.

![.center.medium.p2](./guide%20images/solder%20steps/3.JPG)

![.center.medium.p2](./guide%20images/solder%20steps/4.JPG)

Solder it in place and snip off the excess legs.

![.center.medium](./guide%20images/solder%20steps/5.JPG)

Next is the LED itself. Solder it directly following the same approach as the resistor or use a connector like i did.

![.center.medium](./guide%20images/solder%20steps/6-a.JPG)

**Take note of polarity** LED's are polarised. Take note of the **+** marked on the PCB.

Lastly decide if you wish to fit a removable jumper or use the solder bridge.

Below you can see the solder bridge being used.

![.center.large](./guide%20images/solder%20steps/7-a.JPG)

<div class="page"/>

# 2.2 - Powering the Arduino

As with the LED a jumper or solder bridge can be used. Now is the time to decide which.

Below you can see the jumper being used.

![.center.large](./guide%20images/solder%20steps/10-a.JPG)

<div class="page"/>

# 2.3 - PSP Analog Stick

The PSP's analog stick is connected to the PCB via header pins. Wires can be directly soldered if desired but that would make assembly difficult.

I chose to use header pins.

---

ℹ **Soldering to the PSP**

Soldering to the PSP's analog stick points will be described later in this guide.

---

&nbsp;

![.center.large](./guide%20images/solder%20steps/9.JPG)

<div class="page"/>

# 2.4 - Providing power

It is expected you will implement you own power solution. The PCB provides pin headers for Power in and Power out to the PSP.

If you choose to solder cables directly to the PCB there are mounting holes you can route cable ties through to help secure those cables.

---

⚠ **There is no power regulation**

The PCB does not regulate the power input. You are expected to provide a **stable 5v**. Any more or less will damage the PSP and the Arduino.

⚠ **Take note of polarity**

Positive pins are both on the inside with negative on the outside.

---

I chose to use the same PCB connectors as the LED. If choosing connectors be sure they can handle the load the PSP will put on them.

![.center.large](./guide%20images/solder%20steps/10-b.JPG)

<div class="page"/>

# 2.5 - Fitting the Arduino

The Arduino is fitted with its USB facing the edge of the board.

---

ℹ In the images there is **USB** written on the board, this was a design error. **The USB should face to the outside edge of the board**

---

However you choose to solder the Arduino double check all soldering points as its easy to miss some.

In the following image there are multiple pins which did not solder properly both for the Arduino and the MCP4251.

![](./guide%20images/solder%20steps/14-a.JPG)

<div class="page"/>

# 2.6 - Fitting the MCP4251

When fitting the MCP4251 be sure to orientate it correctly. There is a little dot on the top of the chip, this should face the the same side as the notch and marking **u1** on the board.

![.center.medium.p1](./guide%20images/solder%20steps/10-c.JPG)
![.center.medium.p1](./guide%20images/solder%20steps/13.JPG)

<div class="page"/>

# 2.7 - Providing power to the PSP.

I chose to provide power to the PSP by buying a cheap replacement power cable for the PSP and then chopping it in halve.

The end with the yellow barrel jack i attached to the power in connector, the other half to the PSP power out.

Be sure to check polarity with a multi meter, however i found the white wire in the below image to be positive.

---

ℹ Don't forget to put heat shrink on cables **before** soldering. Unlike me.

---

&nbsp;

![.center.large](./guide%20images/solder%20steps/11.JPG)

<div class="page"/>

# Assembly Finished

Your PSP Consolizer should now be fully assembled and looking something like this.

![.center.large.p1](./guide%20images/solder%20steps/15.JPG)
![.center.large.p1](./guide%20images/solder%20steps/17.JPG)
![.center.large.p1](./guide%20images/solder%20steps/16.JPG)

<div class="page"/>

# 3 - Programming the Arduino

The arduino can be programmed before or after installing it into the PSP Consolizer. Launching the **psp-consoliser-programmer.exe** application downloaded from the release package.

It will tell you what steps need to be followed next.

<div class="page"/>

# 4 - Connecting the Arduino and PSP

The Arduino is connected to the PSP via 3 FFC cables and soldering four wires for the Analog stick.

Connect the FFC's to the connectors on the PSP below. **Left** goes to **L Conn.**, **Right** to **R Conn.** and **Center** to **C Conn.**.

![.center.large.p1](./guide%20images/solder%20steps/mobo.JPG)

Don't forget to connect the power cable.

## 4.1 - Soldering the Analog Stick.

Next you need to solder to the following pins on the back of the PCB. These look like nice and easy large traces on the PCB. **They are not**

![.center.large.p1](./guide%20images/solder%20steps/analog.png)

They are small metal tabs tack soldered to two small pads underneath on either side of them.

When you go anywhere near them the gold pads will lift.

Lots of flux will help here and surface tension will help to stop solder bridges. If you do get bridges use solder wick to clean up it all up and try again. Do not put too much heat into the board as you can easily list the small traces underneath. Lots of flux will help.

Once finished it should look something like this (my soldering is not great).

![.center.large.p1](./guide%20images/solder%20steps/pins.jpg)

<div class="page"/>

This is then connected on the headers soldered to the PCB earlier.

They are connected in order, with the black wire in the top image being connected to the left most pin on the connector below and the purple wire connected to the right pin.

![.center.large](./guide%20images/solder%20steps/9.JPG)

The PSP is now fully connected and you should be complete.

The first time you turn Consolizer on you will need to press the sync button to sync a controller. Pressing this button will cause the Consolizer to forget any previously connected controller.

If a controller is connected it will not disconnect.

![.center.large](./guide%20images/done.JPG)

<div class="page"/>

**END OF DOCUMENT**
