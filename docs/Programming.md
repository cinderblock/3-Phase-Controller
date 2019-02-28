# Calibration Procedure

So, you got a motor.
You need to program it and/or calibrate it.

## Load bootloader

First up, load the bootloader.
We're using a lightly modified reference bootloader from LUFA.
Source code can be found in [`bootloader`](../bootloader).

### Hardware

Any ISP based programmer should work pretty easily.
The main build script generates a `.hex` file.

Cheapest (and also excellent) AVR ISP programmer is: [USB µISP](https://www.tindie.com/products/nsayer/usb-uisp/).

The [hardware](https://github.com/cinderblock/3-Phase-Driver) is designed to be programmed with this a standard 6-pin pogo connector used in the ARM world.
The cheapest I've found are from [Tag-Connect](http://www.tag-connect.com/TC2030-IDC-NL) _([alternate](http://www.tag-connect.com/TC2030-IDC-NL-10))_.

#### Setup

1. Connect **USB µISP** to computer
1. Set switch on **USB µISP** to `5v`
1. Connect **Tag-Connect** cable to **USB µISP**
1. Find programming pads and test connection

#### Build and load

Just need to run `make` in the `bootloader` directory on your computer.

```bash
cd bootloader
make
```

_You could setup a script in your IDE to call `make` instead._

The first build will take possibly a minute.
At the end, it will erase and load the bootloader onto the motor.
The actual programming only takes a couple seconds, during which time the **Tag-Connect** cable needs to be held onto the PCB.

If the programming fails or you connect the pins in time, just run `make` again.
It will run nearly instantaneously as the previous build will be cached.

## Firmware

### Hardware Setup

1. Connect USB
1. Connect Main power (not needed for un-isolated hardware)
1. Press reset
1. LED will flash @ 1Hz (bootloader mode)

### Firmware Setup

For now, we are hard coding some constants related to the particular motor.
Currently the firmware is only setup for the couple motors that I'm playing with.
Update [`src/Config.hpp`](../src/Config.hpp) and define one of `QUANTUM_DRIVE`, `BED_CONTROLLER`, or `HOVER_DRIVE`.
_Search the code for those values to see the few places they are used for other customizations._

### Load

1. Run `make` in the top level directory

_Well that was easy!_

## Calibration

### Hardware Setup

1. Connect USB
1. Connect Main power

### Software Setup

Run `yarn` in the `Calibration-Tool` directory to install needed dependencies.
_Installing `node` or `yarn` are outside the scope of this document._

#### Edit Calibration Constants

Software is not yet smart enough to automatically sync this.
Soon...

Search for `Main Calibration Constants` and edit away.

### Run Calibration Tool

```bash
yarn start
```

Tool will print status updates periodically.
It will run all the way forward for N rotations, overshoot a little, and return.
_(`0% - c` -> `100% + c` -> `0%`)_

At the end, it will generate some charts of the data as PNG files.

It will also generate a `.hex` file that can be loaded onto the attached motor.
It will then be calibrated and ready to go.

## TODO:

Describe changing serial number
