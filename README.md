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
| `bom.csv` | Bill of materials |

## Use Guide

After building all physical hardware, download the firmware folder then:

```cmd
export PICO_SDK_PATH=/path/to/pico-sdk

cd firmware && mkdir build && cd build

cmake .. && make
```

hold BOOTSEL on the Pico, plug in the USB and drag qbmx.uf2 to the mass storage device.

OLED Shows 5 pages:
0. Current keyboard layer
1. Custom text/bitmap
2. LED mode + brightness
3. WPM counter with bar graph
4. Host temps (CPU avg, GPU avg, Pico internal temp)

For temperature tracking to work properly,
1. Install pyserial
```cmd
pip3 install pyserial
```
2. Run host script which auto-detects the Pico port
```cmd
python3 firmware/host_temp.py
```

> **Note:** osx-cpu-temp will return 0.0°C on Apple Silicon Macs without SMC entitlements; where the OLED will show N/A in this case. You may instead compile `osx-cpu-temp` from source or use  `sudo powermetrics`.

### Programmable Keys (Column 14, Rows 0-5)
By default they are mapped to F13-F18 (USB HID keycodes 0x68 -> 0x6D), which is recognised by every major OS. It can be remapped in system settings.

Other functions include:
Short press rotary encoder to pause, Long press rotary encoder to toggle between volume and brightness (backlight) mode

## Build Log / Journal

See [`Journal.md`](Journal.md).

## BOM

See [`bom.csv`](bom.csv).

## Acknowledgements

- [Hack Club Keeb](https://keeb.hackclub.com/) for the program
- siderakb/key-switches.pretty and eblaster/marbastlib libraries
- GrabCAD models [switch](https://grabcad.com/library/low-profile-cherry-mx-red-1), [encoder](https://grabcad.com/library/11mm-metal-shaft-rotary-encoders-tht-vertical-w-push-on-switch-1), [OLED](https://grabcad.com/library/0-91-128x32-oled-display-1)
- Github models [keycaps](https://github.com/anhthang/dsa-keycap/)
