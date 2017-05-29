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
u2 ThreePhaseController::lastAlpha;

void TIMER4_OVF_vect() {
  ThreePhaseController::isr();
}

void ThreePhaseController::isr() {
  u1 static mlx = 1;

  // Automatically start MLX communications every few ISRs.
  if (!--mlx) {
    MLX90363::startTransmitting();
    mlx = cyclesPWMPerMLX;
  }
  
  // Only if the controller is enabled will we update the outputs
  if (enabled) updatePhaseAngle();
}

void ThreePhaseController::updatePhaseAngle() {
  // Scale phase to output range
  u2 outputPhase = ThreePhasePositionEstimator::predictPhase();

  if (ServoController::isUpdating()) {
    // Offset from current angle by 90deg for max torque

    if (!isZeroTorque) {

      //depending on which direction is forward shift by 90 degrees
      if (Config::forward) {
        if (isForwardTorque) outputPhase -= ThreePhaseDriver::StepsPerCycle / 4;
        else outputPhase += ThreePhaseDriver::StepsPerCycle / 4;
      } else {
        if (isForwardTorque) outputPhase += ThreePhaseDriver::StepsPerCycle / 4;
        else outputPhase -= ThreePhaseDriver::StepsPerCycle / 4;
      }

      // Fix outputPhase range

      //depending on which direction is forward correct the number
      if (Config::forward) {
        if (outputPhase >= ThreePhaseDriver::StepsPerCycle) {
          // Fix it
          if (isForwardTorque)outputPhase += ThreePhaseDriver::StepsPerCycle;
          else outputPhase -= ThreePhaseDriver::StepsPerCycle;
        }
      } else {
        if (outputPhase >= ThreePhaseDriver::StepsPerCycle) {
          // Fix it
          if (isForwardTorque)outputPhase -= ThreePhaseDriver::StepsPerCycle;
          else outputPhase += ThreePhaseDriver::StepsPerCycle;
        }
      }

    }

    // Update driver outputs
    ThreePhaseDriver::advanceTo(outputPhase);
  }
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

bool ThreePhaseController::updateDriver() {
  if (!MLX90363::hasNewData(magRoll)) return false;

  // We can always grab the latest Alpha value safely here
  lastAlpha = MLX90363::getAlpha();
  auto const magPha = Lookup::AlphaToPhase(lastAlpha);

  roll++;

  ThreePhasePositionEstimator::freshPhase(magPha);

  return true;
}
