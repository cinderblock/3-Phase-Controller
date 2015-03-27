## Creator: Cameron Tacklind
##
## This is the main Makefile for building AVR projects. It should only be edited
## to add generic and project specific features. Ideally, there are no real targets
## here. Just variables, or overrides, that the included helper makefiles will use.

# List of C file basenames to build
C = 

# List of CPP file basenames to build
CPP = main Board SPI MLX90363 ThreePhaseDriver Debug

CPP += TwillBotInterface TripleBuffer-impl

# Select specific LUFA source files to compile like this
#LUFA_SRC = LUFA/Drivers/USB/Class/Common/HIDParser.c

F_CPU = 16000000UL

#BLD_STD_GCC ?= c11
#BLD_STD_GXX ?= c++11

# Registers used by ThreePhaseDriver
BLD_FLAGS_EXTRA=-ffixed-2 -ffixed-3 -ffixed-4 -ffixed-5 -ffixed-6

TARGET = turnigy

MCU = atmega32u4

all: build-lss asm run
run: dfu-erase dfu-flash dfu-reset
	
ASM = #$(CPP:%=%.cpp.S)

# Load local settings
-include local.mk

# Force setting certain make flags
include $(UMAKER)tools/makeflags.mk

# Optional configuration testing for development
include $(UMAKER)tools/checks.mk

# Defs for our setup
include $(UMAKER)vars/AVR.mk


# Library targets
#include $(UMAKER)tools/AVR/lufa.mk
#include $(UMAKER)tools/extlib.mk
include $(UMAKER)tools/AVR/AVR++.mk

# Build targets
include $(UMAKER)tools/build.mk

# Intermediate assembly
include $(UMAKER)tools/assembly.mk

# Programmer targets
include $(UMAKER)tools/dfu.mk
#include $(UMAKER)tools/nrfjprog.mk
#include $(UMAKER)tools/AVR/avrdude.mk

# Directory creation targets
include $(UMAKER)tools/mkdir.mk

.PHONY: all run
