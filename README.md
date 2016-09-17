# Twill Firmware for Turnigy Driver

3 Phase motor controller driver firmware.

## Getting Started

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
