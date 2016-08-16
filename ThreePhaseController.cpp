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
#include "Predictor.h"
#include "LookupTable.h"
#include "ServoController.h"
#include "Config.h"

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

  // Scale phase to output range
  u2 outputPhase = Predictor::predictPhase();

  if(ServoController::isUpdating()){ 
    // Offset from current angle by 90deg for max torque
    if (isZeroTorque);
    else{
      //depending on which direction is forward shift by 90 degrees
      if(Config::forward){
        if (isForwardTorque) outputPhase -= ThreePhaseDriver::StepsPerCycle / 4;
        else outputPhase += ThreePhaseDriver::StepsPerCycle / 4;
      }
      else{
        if (isForwardTorque) outputPhase += ThreePhaseDriver::StepsPerCycle / 4;
        else outputPhase -= ThreePhaseDriver::StepsPerCycle / 4;
      }
    }

    // Fix outputPhase range
    if (isZeroTorque);
    else{
      //depending on which direction is forward correct the number
      if(Config::forward){
        if (outputPhase >= ThreePhaseDriver::StepsPerCycle) {
        // Fix it
          if (isForwardTorque)outputPhase += ThreePhaseDriver::StepsPerCycle;
          else outputPhase -= ThreePhaseDriver::StepsPerCycle;
        }
      }
      else {
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
  
  // Don't continue if we're not done counting down
  if (--mlx)
    return;

  MLX90363::startTransmitting();

  mlx = cyclesPWMPerMLX;
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
  Predictor::init(Lookup::AlphaToPhase(MLX90363::getAlpha()));
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

  Predictor::freshPhase(magPha);

  return true;
}
