
#include "ServoController.hpp"
#include "ThreePhase/Controller.hpp"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

ServoController::Mode ServoController::servoMode = Mode::Init;

ThreePhaseController::Amplitude ServoController::amplitudeCommand(0);
s2 ServoController::velocityCommand;
ThreePhaseDriver::PhasePosition ServoController::positionCommand;

u2 ServoController::position_P = 0;
u2 ServoController::position_I = 0;
u2 ServoController::position_D = 0;

u2 ServoController::velocity_P = 0;
u2 ServoController::velocity_I = 0;
u2 ServoController::velocity_D = 0;

u1 ServoController::positionShift;
u1 ServoController::velocityShift;

void ServoController::init() {
  ThreePhaseController::init();

  servoMode = Mode::Init;

  position_P = 0;
  position_I = 0;
  position_D = 0; //(float) .12  = 61/512

  positionShift = 12;
}

void ServoController::update() {
  // This function is called as often as possible in the main loop.
  // It looks at its currently available data and calculates a signed amplitude
  // to pass back to the ThreePhaseController

  if (servoMode == Mode::Init) {
    // DO NOTHING
  } else if (servoMode == Mode::Amplitude) {
    ThreePhaseController::setAmplitudeTarget(amplitudeCommand);

  } else if (servoMode == Mode::Velocity) {
    static s2 lastVel = 0;
    static ThreePhaseController::Amplitude lastCommand(0);
    const s2 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s2 velocityDelta = vel - lastVel;

    const s2 velocityError = vel - velocityCommand;

    s4 command = lastCommand + ((velocityError * velocity_P + velocityDelta * velocity_D) >> velocityShift);

    lastCommand = command;

    ThreePhaseController::setAmplitudeTarget(command);

    lastVel = vel;

  } else if (servoMode == Mode::Position) {
    const s4 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s4 positionError = positionCommand - ThreePhasePositionEstimator::getMagnetometerPhaseEstimate();

    const s4 command = ((positionError * position_P) >> 16) - ((vel * position_D) >> 8);

    ThreePhaseController::setAmplitudeTarget(command);

  } else {
  }
}

void ServoController::setAmplitude(s2 amplitude) {
  servoMode = Mode::Amplitude;
  amplitudeCommand = amplitude;
}

void ServoController::setVelocity(s2 velocity) {
  servoMode = Mode::Velocity;
  velocityCommand = velocity;
}

void ServoController::setPosition(ThreePhaseDriver::PhasePosition position) {
  servoMode = Mode::Position;
  positionCommand = position;
}

void ServoController::setDistance(s2 dist) {
  servoMode = Mode::Position;
  if (dist >= 0)
    positionCommand += (u2)dist;
  else
    positionCommand -= (u2)(-dist);
}

void ServoController::setEnable(bool enable) {
  if (enable)
    return;

  servoMode = Mode::Init;
}
