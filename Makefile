## Creator: Cameron Tacklind
##
## This is the main Makefile for building AVR projects. It should only be edited
## to add generic and project specific features. Ideally, there are no real targets
## here. Just variables, or overrides, that the included helper makefiles will use.

# List of C file basenames to build
C = 

# List of CPP file basenames to build
CPP = main Board Debug Timer Clock

CPP += ThreePhaseDriver
CPP += MLX90363 ThreePhasePositionEstimator ThreePhaseController

CPP += ServoController

CPP += MLXDebug

CPP += TripleBuffer-impl BlockBuffer-impl

CPP += Demo
CPP += Calibration

CPP += HallWatcher

#CPP += LookupTable/$(MotorID)

CPP += LookupTable

CPP += SerialInterface

# This should be compiled by AVR++ but it's a little broken so do this instead.
CPP += AVR++/gccGuard AVR++/USART

#MotorID = Motor1

AVRpp_SRC = TimerTimeout #ADC USART  gccGuard
libCameron_SRC = CRC8 DecPrintFormatter

# Select specific LUFA source files to compile like this
#LUFA_SRC = LUFA/Drivers/USB/Class/Common/HIDParser.c

F_CPU = 16000000UL

#BLD_STD_GCC ?= c11
#BLD_STD_GXX ?= c++11

# Registers used by ThreePhaseDriver
BLD_FLAGS_EXTRA=-ffixed-2 -ffixed-3 -ffixed-4 -ffixed-5 -ffixed-6

TARGET = turnigy

MCU = atmega32u4

all: build-lss run
run: dfu-erase dfu-flash dfu-reset

#ASM = $(CPP:%=%.cpp.S)

# Load local settings
-include local.mk

uMakerPath ?= uMaker/
AVRpp_BASEDIR ?= AVR++/

# Force setting certain make flags
#include $(uMakerPath)tools/makeflags.mk

# Optional configuration testing for development
include $(uMakerPath)tools/checks.mk

# Defs for our setup
include $(uMakerPath)vars/AVR.mk


# Library targets
#include $(uMakerPath)tools/AVR/lufa.mk
#include $(uMakerPath)tools/extlib.mk
include $(uMakerPath)tools/AVR/AVR++.mk
include $(uMakerPath)tools/AVR/libCameron.mk

# Build targets
include $(uMakerPath)tools/build.mk

# Intermediate assembly
include $(uMakerPath)tools/assembly.mk

# Programmer targets
include $(uMakerPath)tools/dfu.mk
#include $(uMakerPath)tools/nrfjprog.mk
#include $(uMakerPath)tools/AVR/avrdude.mk

# Directory creation targets
include $(uMakerPath)tools/mkdir.mk

.PHONY: all run
