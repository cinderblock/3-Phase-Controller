/* 
 * File:   ThreePhaseController.cpp
 * Author: Cameron
 * 
 * Created on October 22, 2015, 2:21 AM
 */

#include <util/atomic.h>
#include <util/delay.h>

#include "ThreePhaseController.h"
#include "ThreePhaseDriver.h"
#include "Debug.h"
#include "Interpreter.h"
#include "ThreePhasePositionEstimator.h"
#include "LookupTable.h"
#include "ServoController.h"
#include "Config.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

volatile bool ThreePhaseController::enabled = false;
bool ThreePhaseController::isForwardTorque;

/**
 * This interrupt is triggered on TIMER4 (PWM6) overflow. This happens at the BOTTOM
 * of the dual slope timer. New OCR values are latched at BOTTOM as well.
 *
 * We use this to calculate new PWM values on a periodic interval. We also use
 * a multiple of this interval to trigger new MLX readings.
 */
void TIMER4_OVF_vect() {
//  Board::SER::Tx::on();
  ThreePhaseController::controlLoop();
//  Board::SER::Tx::off();
}

void ThreePhaseController::controlLoop() {
  // Advance our position estimate on step in time
  ThreePhaseDriver::PhasePosition p = ThreePhasePositionEstimator::advance();

  // TODO: more phase advance at higher speeds
  if (isForwardTorque == Config::forward) {
    p += output90DegPhaseShift;
  } else {
    p -= output90DegPhaseShift;
  }

  ThreePhaseDriver::advanceTo(p);
}

void ThreePhaseController::init() {
  ThreePhaseDriver::init();
  ThreePhaseDriver::setAmplitude(0);

  ThreePhasePositionEstimator::init();

  // Let the controlLoop() run
  enable();
}

void ThreePhaseController::setAmplitude(const Amplitude t) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    isForwardTorque = t.forward;
    ThreePhaseDriver::setAmplitude(t.amplitude);
  }
}