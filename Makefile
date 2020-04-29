## Creator: Cameron Tacklind
##
## This is the main Makefile for building AVR projects. It should only be edited
## to add generic and project specific features. Ideally, there are no real targets
## here. Just variables, or overrides, that the included helper makefiles will use.

# List of C file basenames to build
cNames =

# List of CPP file basenames to build
cppNames = main Debug
cppNames += System/Board
cppNames += System/Clock

cppNames += AVR/Timer

cppNames += ThreePhase/Driver
cppNames += Sensors/MLX90363 ThreePhase/PositionEstimator ThreePhase/Controller

cppNames += ThreePhase/FaultTester

cppNames += ServoController

cppNames += Sensors/MLXDebug

cppNames += GenericImplementations/TripleBuffer-impl
cppNames += GenericImplementations/BlockBuffer-impl

cppNames += Routines/Demo
cppNames += Routines/Calibration

cppNames += Sensors/HallWatcher

cppNames += CalibrationData/LookupTable
cppNames += CalibrationData/idBlock

cppNames += Interface/Serial

cppNames += Interface/Interface

cppNames += Interface/USB/Interface

cppNames += Interface/USB/Descriptors
cNames += Interface/USB/Descriptors

#MotorID = Motor1

AVRpp_SRC = TimerTimeout USART gccGuard FlashCRC
cppNames += Sensors/ScanningADC-impl

libCameron_SRC = CRC8 DecPrintFormatter

# Select specific LUFA source files to compile like this
#LUFA_SRC = LUFA/Drivers/USB/Class/Common/HIDParser.c

F_CPU = 16000000UL

DEFINES = USE_LUFA_CONFIG_HEADER

# LUFA needs this with USE_LUFA_CONFIG_HEADER so that it can find LUFAConfig.h
INCLUDES = src

# Make compilation fail on first error
Build_Flags_Extra = -Wfatal-errors

TARGET = controller

MCU = atmega32u4

all: build-lss

# Program with AVR attached locally
all: dfu-flash dfu-reset

#ASM = $(CPP:%=%.cpp.S)

-include remote-config.mk
MAKEFILE_LIST := $(filter-out remote-config.mk,$(MAKEFILE_LIST))

# Load local settings
-include local.mk
-include local.$(shell hostname).mk

uMakerPath ?= uMaker/
Source_Path ?= src/

include $(uMakerPath)tools/paths.mk

# Generate list of source files from basenames
include $(uMakerPath)tools/source.mk

# Force setting certain make flags
include $(uMakerPath)tools/makeflags.mk

# Optional configuration testing for development
include $(uMakerPath)tools/checks.mk

# Defs for our setup
include $(uMakerPath)vars/AVR.mk


# Library targets
include $(uMakerPath)tools/AVR/lufa.mk
#include $(uMakerPath)tools/extlib.mk
include $(uMakerPath)tools/AVR/AVR++.mk
include $(uMakerPath)tools/libCameron.mk

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

REMOTE_HEX = $(TARGET).hex
REMOTE?=sleepypi-04

remote: $(OUT_HEX)
	$(ECO) Sending $(OUT_HEX) to $(REMOTE)
	pscp -q $(OUT_HEX) $(REMOTE):$(REMOTE_HEX)
	-plink -batch $(REMOTE) sudo $(DFU_TARGETED) flash $(REMOTE_HEX) \&\& sudo $(DFU_TARGETED) reset
	plink -batch $(REMOTE) rm $(REMOTE_HEX)

remote4: $(OUT_HEX)
	$(ECO) Sending $(OUT_HEX) to $(REMOTE)
	pscp -q $(OUT_HEX) $(REMOTE):$(REMOTE_HEX)
	-plink -batch $(REMOTE) sudo $(DFU_TARGETED) flash $(REMOTE_HEX) \&\& sudo $(DFU_TARGETED) reset
	-plink -batch $(REMOTE) sudo $(DFU_TARGETED) flash $(REMOTE_HEX) \&\& sudo $(DFU_TARGETED) reset
	-plink -batch $(REMOTE) sudo $(DFU_TARGETED) flash $(REMOTE_HEX) \&\& sudo $(DFU_TARGETED) reset
	-plink -batch $(REMOTE) sudo $(DFU_TARGETED) flash $(REMOTE_HEX) \&\& sudo $(DFU_TARGETED) reset
	plink -batch $(REMOTE) rm $(REMOTE_HEX)

.PHONY: all run remote_prog remote remote4
