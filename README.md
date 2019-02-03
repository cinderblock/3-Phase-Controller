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
- [x] USB Communication

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

### Build Requirements

Only real requirement is running the main `Makefile`.
All build steps are configurable on a per machine hostname basis.
Create a file named `local.<hostname>.mk` in the project root, next to the main `Makefile`.
This file will be automatically loaded and can be used to override any make variables.
The primary use for this is to enable the ability to not have all the needed binaries in the PATH.

#### Compiler

While we endeavour to be standards compliant, the reality is that it is difficult to ensure all possible compilers work.

Known working compilers:

- [AVR 8-bit Toolchain](https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers) v3.6.2

#### Common `local.<hostname>.mk` options

```make
GCC_RootDir = C:/Program Files (x86)/avr8-gnu-toolchain
mkdirCommand = mkdir
```

#### Windows

All paths in uMaker use `/` as a directory separator.

Windows's `mkdir`/`md` does not support `/` directory separator.

Installing cygwin is the easiest way to get building on Windows working.
Make sure `make` is "selected for installation".

### Bootloader

The current Makefile is setup to program an AVR via USB DFU. It is possible to switch to using an ISP or other bootloader/programmer easily as well.

## Associated documentation

- [Technical Docs](docs/README.md)
- [3 Phase sine generation](docs/3%20Phase%20Sine%20Wave%20Generation.md)
- [Triple Buffer](libCameron/Triple%20Buffer.png)
