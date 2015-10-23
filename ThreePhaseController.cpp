/* 
 * File:   ThreePhaseController.cpp
 * Author: Cameron
 * 
 * Created on October 22, 2015, 2:21 AM
 */

#include <avr/pgmspace.h>

#include "ThreePhaseController.h"
#include "MLX90363.h"
#include "ThreePhaseDriver.h"

s2 ThreePhaseController::position;
s2 ThreePhaseController::velocity;
bool ThreePhaseController::isForward;

/**
 * 14-bit lookup table for magnetometer Alpha value to Phase value
 */
static const u1 AlphaToPhaseLookup[20480] PROGMEM = {};

inline static u2 lookupAlphaToPhase(const u2 alpha) {
 return pgm_read_word(&AlphaToPhaseLookup[alpha]);
}

void ThreePhaseController::init() {
 MLX90363::init();
 ThreePhaseDriver::init();
 position = 0;
 velocity = 0;
 ThreePhaseDriver::setAmplitude(0);
}

void ThreePhaseController::setTorque(const Torque t) {
 isForward = t.forward;
 ThreePhaseDriver::setAmplitude(t.amplitude);
 lookupAlphaToPhase(t.amplitude);
}

void ThreePhaseController::updateDriver() {
 u2 pos = position;
 
 if (isForward)
  pos += drivePhaseShift;
 else
  pos += ThreePhaseDriver::StepsPerCycle - drivePhaseShift;
 
 pos %= ThreePhaseDriver::StepsPerCycle;
 
 ThreePhaseDriver::advanceTo(pos);
}
