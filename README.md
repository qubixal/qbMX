# qbMX

> A custom mechanical keyboard built from scratch for [Hack Club Keeb](https://keeb.hackclub.com/) YSWS.

![Full Render](readme-library/qbMX.png)

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

See [`Journal.md`](Journal.md).

## BOM

See [`bom.csv`](bom.csv).

## Acknowledgements

- [Hack Club Keeb](https://keeb.hackclub.com/) for the program
- siderakb/key-switches.pretty and eblaster/marbastlib libraries
- GrabCAD models [switch](https://grabcad.com/library/low-profile-cherry-mx-red-1), [encoder](https://grabcad.com/library/11mm-metal-shaft-rotary-encoders-tht-vertical-w-push-on-switch-1), [OLED](https://grabcad.com/library/0-91-128x32-oled-display-1)
- Github models [keycaps](https://github.com/anhthang/dsa-keycap/)
