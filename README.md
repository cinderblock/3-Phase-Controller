# Twill Firmware for Turnigy Driver

3 Phase motor controller driver firmware.

## Getting Started

### Board

This code is ment to run on the [ATmega32U4](http://www.atmel.com/Images/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf) on [3 Phase Driver Board](https://github.com/cinderblock/3-Phase-Driver).

## Motor

Uses the [Turnigy 480](http://www.hobbyking.com/hobbyking/store/__19038__Turnigy_Park480_Brushless_Outrunner_1320kv.html).

### Git Submodules

After cloning this repo, you'll need to get the git submodules:

```
git submodule init
git submodule update
```

### Bootloader

The current Makefile is setup to program an AVR via USB DFU. It is possible to switch to using an ISP or other bootloader/programmer easily as well.

## Associated documentation

- [Technical Docs](docs/README.md)
- [3 Phase sine generation](https://docs.google.com/spreadsheets/d/1I45kGhncSQvR4_B_AG72Bqk7MJlNRIvBI-JD9qAgE8U/edit?usp=sharing)
- [Triple Buffer](https://drive.draw.io/#G0Bzw2V8vLyJThV1dJRTZ0dXRsODQ)
