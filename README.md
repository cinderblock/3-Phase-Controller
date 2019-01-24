# Twill Firmware for Turnigy Driver

3 Phase motor controller driver firmware.

## Current Implementation

This is very much a work in progress. The following list is of currently implemented and planned features.

- [ ] Sensors
  - [x] Magmentometer
  - [ ] Back EMF Sensing
  - [ ] External Sensor
- [x] Motor movement
  - [ ] Full Torque
- [x] Commands
  - [x] Stepping function
  - [x] Amplitude
  - [x] Velocity
  - [x] Position
- [x] I2C Communication
- [ ] USB Communication

## Getting Started

### Board

This code is meant to run on the [ATmega32U4](http://www.atmel.com/Images/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf) on [3 Phase Driver Board](https://github.com/cinderblock/3-Phase-Driver).

## Motor

Currently Testing with [Quanum MT5206](https://hobbyking.com/en_us/quanum-mt-series-5206-320kv-brushless-multirotor-motor-built-by-dys.html).
Also playing with Hoverboard motors.

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
- [3 Phase sine generation](docs/3%20Phase%20Sine%20Generation.md)
- [Triple Buffer](libCameron/Triple%20Buffer.png)
