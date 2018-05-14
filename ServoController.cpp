
#include "ServoController.h"
#include "ThreePhaseController.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

ServoController::Mode ServoController::servoMode = Mode::Init;

s2 ServoController::amplitudeCommand;
s4 ServoController::driveAmplitudeScaled;
s2 ServoController::velocityCommand;
s4 ServoController::positionCommand;
s2 ServoController::onRotation;

u1 ServoController::positionP;
u1 ServoController::positionI;
u1 ServoController::positionD;

u1 ServoController::velocityP;
u1 ServoController::velocityI;
u1 ServoController::velocityD;

u1 ServoController::positionShift;
u1 ServoController::velocityShift;

/**
 * Distance function with a wrap around
 */
s2 dist(u2 to, u2 from, const u2 wrapdist) {

  s2 delta = (s2)to - (s2)from;

  if (delta > (s2)(wrapdist / 2)) {
    delta = delta - ((s2)wrapdist);
  } else if (delta < -((s2)wrapdist / 2)) {
    delta = ((s2)wrapdist) + delta;
  }

  return delta;

}

s2 abs2(s2 num) {
  if (num < 0) return -num;
  return num;
}

void ServoController::init() {
  ThreePhaseController::init();

  servoMode = Mode::Init;

  onRotation = 0;

  positionP = 200;
  positionI = 0;
  positionD = 0;

  positionShift = 12;

  amplitudeCommand = 0;
  driveAmplitudeScaled = 0;
}

void ServoController::update() {
  // This function is called as often as possible in the main loop.
  // It looks at its currently available data and calculates a signed amplitude to pass back to the ThreePhaseController

  if (servoMode == Mode::Init) {
    // DO NOTHING
  } else if (servoMode == Mode::Amplitude) {
    ThreePhaseController::setAmplitude(amplitudeCommand);

  } else if (servoMode == Mode::Velocity) {
    static s2 lastVel = 0;
    const s2 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s2 velocityDelta = vel - lastVel;

    const s2 velocityError = vel - velocityCommand;

    s4 command = ThreePhaseController::getAmplitude() + ((velocityError * velocityP + velocityDelta * velocityD) >> velocityShift);

    constexpr s4 MAX = ThreePhaseDriver::maxAmplitude;
    if (command > MAX) {
      command = MAX;
    } else if (command < -MAX) {
      command = -MAX;
    }

    ThreePhaseController::setAmplitude((s2)command);

    lastVel = vel;

  } else if (servoMode == Mode::Position) {

    const s2 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s4 distance = positionCommand - getPosition();

    s4 command = (distance * positionP + vel * positionD) >> positionShift;

    constexpr s4 MAX = ThreePhaseDriver::maxAmplitude;
    if (command > MAX) {
      command = MAX;
    } else if (command < -MAX) {
      command = -MAX;
    }

    ThreePhaseController::setAmplitude((s2)command);

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

void ServoController::setPosition(s4 position) {
  servoMode = Mode::Position;

  positionCommand = position;
}

void ServoController::setDistance(s4 dist) {
  servoMode = Mode::Position;

  positionCommand = getPosition() + dist;
}

void ServoController::setEnable(bool enable) {
  if (!enable)
    servoMode = Mode::Init;
}
