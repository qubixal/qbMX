# qbMX

> A custom mechanical keyboard built from scratch for [Hack Club Keeb](https://keeb.hackclub.com/) YSWS.

![PCB render](readme-library/pcb.png)

## Overview

**qbMX** is a fully custom mechanical keyboard designed in KiCad and Fusion. It features:

- **85-key (79+6) layout** with a rotary encoder, modelled after Apple's Magic Keyboard.
- **Cherry MX Low-Profile Red** switches, and on-plate stabalisers (instead of on-pcb)
- Uses the **RP2040 microcontroller** (Orpheus Pico)
- A 0.91" **128x32 OLED display**
- 15 mini-LEDs at the top of the keyboard
- Custom PCB, ordered and assembled by me

## Why I Built qbMX

I wanted to learn about and build a keyboard myself, since I'd never done it before.
Designing the matrix and optimising GPIO lanes with functionality was a cool challenge. Oh, and I just wanted a keyboard; and Keeb gave me an excuse to make it.

## What's in This Repo

| Path | Description |
| --- | --- |
| `keeb.kicad_sch` | Schematic |
| `keeb.kicad_pcb` / `keeb.kicad_pro` | KiCad PCB layout & project |
| `pcb/` + `pcb.zip` | Gerbers & drill files (F.Cu, B.Cu, silkscreen, mask, Edge.Cuts) |
| `*.step` | 3D models (board, OLED, rotary encoder, plate) |
| `key-switches.pretty/`, `extra_key.pretty/` | Footprint libraries |
| `readme-library/` | Photos & renders |
| `bom.csv` | Bill of materials _(TODO)_ |

## Build Log / Journal
*note, the KiCad project was not git initialised, and so does not show up under hackatime as a distinct project. The hours are accurate estimates based on daily overviews.

### 1. Planning & schematic —  _5 hrs_
I followed the guide and started planning. Functionalities:
RP2040 microcontroller (), 0.91" OLED screen (2 GPIO), Rotary Encoder (2 GPIO), Keys (21 GPIO, 6 rows 15 columns), "Backlight" (1GPIO) total 26GPIO.

Originally, i designed this for standard size MX switches and did not understand the difference, keyboard sizes (i.e. 60%, 75%, 80%, etc) or keycap u sizes.
![1st](readme-library/first.png)

I handplaced each key which resulted in this taking ages. At this time, I just autorouted to save some time.

### 2. PCB layout — _2 hrs_

Hand-routed the matrix. When I applied the 3D models, I noticed something was off and reconfigured everything.
![2nd](readme-library/second.png)

### 3. Redesign — _6 hrs_

Refactored entire keyboard from scratch after several issues with geographical annotation messing up the keys. Other key changes that happened were:
MX switches -> MX Low Profile Red switches,
On-PCB Stabalisers -> On-Plate Stabalisers (due to lack of footprints available)
![3rd](readme-library/third.png)

Marbastlib was so useless and had nothing, so I had to refactor what I had anyways. Switched to a mix of Marbastlib and siderakb/key-switches.pretty.
To save time placing components, i installed kbplacer. However, i had to retweak every reference number because my LEDs conflicted with diodes, and rotary encoder was counted as a switch. Doing this wouldn't have been a problem if my keyboard wasn't already laid out.

![pcb](readme-library/pcb.png)

I also fetched the 3D models for everything and exported to fusion which took a long time yet again, as I had to find a new rotary encoder with an appropriate height.

![4th](readme-library/fourth.png)

Exported STEP files are available in /prod-3d-models.

So how the heck did creating a rectangle box of doom and despair take 7 hours?
I think it can be explained in one screenshot:
![stupid fusion](readme-library/what.png)

In case you're still clueless, that's fusion taking up 32GB of ram. Apparently having 85 switches and a few more bits ad pieces kills your computer.

I'm still quite happy with the end result. The overall hierarchy is:
------- Top cover (+ acrylic sheet), 0.5mm
keycaps + microcontroller
------- Plate (TP4, in PCB form), 1.2mm
switches and PCB
------- Bottom Case
The overall design is a Top mount keyboard design.
![stupid fusion](readme-library/image.webp)

### 5. Ordering & assembly — _1 hrs_

BOM is currently being finalised.

### 6. Firmware — _X hrs_ _(TODO)_

## BOM

See [`bom.csv`](bom.csv).

## Acknowledgements

- [Hack Club Keeb](https://keeb.hackclub.com/) for the program
- siderakb/key-switches.pretty and eblaster/marbastlib libraries
- GrabCAD models [switch](https://grabcad.com/library/low-profile-cherry-mx-red-1), [encoder](https://grabcad.com/library/11mm-metal-shaft-rotary-encoders-tht-vertical-w-push-on-switch-1), [OLED](https://grabcad.com/library/0-91-128x32-oled-display-1)
- Github models [keycaps](https://github.com/anhthang/dsa-keycap/)
