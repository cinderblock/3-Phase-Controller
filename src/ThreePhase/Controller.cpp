/*
 * File:   ThreePhaseController.cpp
 * Author: Cameron
 *
 * Created on October 22, 2015, 2:21 AM
 */

#include <util/atomic.h>
#include <util/delay.h>

#include "Config.h"
#include "Controller.hpp"
#include "Debug.hpp"
#include "Driver.hpp"
#include "LookupTable.hpp"
#include "PositionEstimator.hpp"
#include "ServoController.hpp"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

volatile bool ThreePhaseController::enabled = false;
volatile bool ThreePhaseController::isForwardTorque;
volatile u1 ThreePhaseController::dampingVelocityGain = 0;
volatile ThreePhaseController::Amplitude ThreePhaseController::targetAmplitude(0);

volatile Atomic<u2> ThreePhaseController::loopCount(0);

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

// This is the loop that happens at 31.25 kHz
void ThreePhaseController::controlLoop() {
  static volatile bool running = false;
  static volatile u1 stepCount = 0;

  stepCount++;
  if (running) {
    return;
  }

  running = true;

  // Record number of steps
  u1 const steps = stepCount;

  // Reset missed step count
  stepCount = 0;

  auto &ref = loopCount.getUnsafe();

  if (ref != 0xffff)
    ref++;

  // Re-enable interrupts
  sei();

  // Advance our position estimate n steps in time
  ThreePhaseDriver::PhasePosition p = ThreePhasePositionEstimator::advance(steps);

  if (enabled) {
    // TODO: more phase advance at higher speeds
    if (isForwardTorque) {
      p += output90DegPhaseShift;
    } else {
      p -= output90DegPhaseShift;
    }

    ThreePhaseDriver::advanceTo(p);
  }

  running = false;
}

void ThreePhaseController::init() {
  ThreePhaseDriver::setAmplitude(0);

  ThreePhasePositionEstimator::init();

  start();
}

void ThreePhaseController::handleNewVelocityEstimate(s2 const v) {
  if (!enabled)
    return;

  const Amplitude t(targetAmplitude + (s2)((s4(v) * dampingVelocityGain) >> 8));
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    isForwardTorque = t.forward;
    ThreePhaseDriver::setAmplitude(t.amplitude);
  }
}
