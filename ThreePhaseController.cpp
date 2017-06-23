/* 
 * File:   ThreePhaseController.cpp
 * Author: Cameron
 * 
 * Created on October 22, 2015, 2:21 AM
 */

#include <util/atomic.h>
#include <util/delay.h>

#include "ThreePhaseController.h"
#include "MLX90363.h"
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
bool ThreePhaseController::isZeroTorque;
u1 ThreePhaseController::magRoll;
u2 ThreePhaseController::roll;


/**
 * This interrupt is triggered on TIMER4 (PWM6) overflow. This happens at the BOTTOM
 * of the dual slope timer. New OCR values are latched at BOTTOM as well.
 *
 * We use this to calculate new PWM values on a periodic interval. We also use
 * a multiple of this interval to trigger new MLX readings.
 */
void TIMER4_OVF_vect() {
  ThreePhaseController::controlLoop();
}

void ThreePhaseController::controlLoop() {
  u1 static mlx = 1;

  // Automatically start MLX communications every few ISRs.
  if (!--mlx) {
    MLX90363::startTransmitting();
    mlx = cyclesPWMPerMLX;
  }

  // Advance our position estimate on step in time
  ThreePhaseDriver::PhasePosition p = ThreePhasePositionEstimator::advance();

  // If we're doing nothing, make sure the outputs are off
  if (isZeroTorque) {
    // TODO: turn off outputs
    return;
  }

  // TODO: more phase advance at higher speeds. Aka FOC.
  if (isForwardTorque == Config::forward) {
    p += u1(ThreePhaseDriver::StepsPerCycle / 4);
  } else {
    p -= u1(ThreePhaseDriver::StepsPerCycle / 4);
  }

  ThreePhaseDriver::advanceTo(p);
}

void ThreePhaseController::init() {
  MLX90363::init();
  ThreePhaseDriver::init();
  ThreePhaseDriver::setAmplitude(0);

  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  // Enable Timer4 Overflow Interrupt
  TIMSK4 = 1 << TOIE4;

  magRoll = MLX90363::getRoll();

  // Get two new readings to get started
  while (!MLX90363::hasNewData(magRoll));
  while (!MLX90363::hasNewData(magRoll));

  roll = 0;
  ThreePhasePositionEstimator::init(Lookup::AlphaToPhase(MLX90363::getAlpha()));
  roll = 1;
}

void ThreePhaseController::setAmplitude(const Amplitude t) {

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    isForwardTorque = t.forward;
    isZeroTorque = t.zero;
    ThreePhaseDriver::setAmplitude(t.amplitude);
  }
}