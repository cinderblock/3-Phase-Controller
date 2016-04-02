
#DFU = "C:/Program Files/WinAVR/bin/dfu-programmer.exe"
# BLD_BIN_PREFIX = C:/Program Files (x86)/avr-gcc-4.9.2_2014-09-12_mingw32/bin/
# BLD_SZE = "C:/Program Files/WinAVR/bin/avr-size" --format=avr --mcu=$(MCU)

-include local.$(shell hostname).mk

all: size