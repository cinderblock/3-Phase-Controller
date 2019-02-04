
#include "ServoController.h"
#include "ThreePhase/Controller.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

ServoController::Mode ServoController::servoMode = Mode::Init;

s2 ServoController::amplitudeCommand;
s4 ServoController::driveAmplitudeScaled;
s2 ServoController::velocityCommand;
u4 ServoController::positionCommand;
s2 ServoController::onRotation;

u1 ServoController::position_P;
u1 ServoController::position_I;
u1 ServoController::position_D;

u1 ServoController::velocity_P;
u1 ServoController::velocity_I;
u1 ServoController::velocity_D;

u1 ServoController::positionShift;
u1 ServoController::velocityShift;

// u2 ServoController::initialPhasePosition;

/**
 * Distance function with a wrap around
 */
s4 wrapdist(u4 to, u4 from) {

  s4 delta = to - from;

  if (delta > s4(ThreePhasePositionEstimator::StepsPerRevolution / 2)) {
    delta -= ThreePhasePositionEstimator::StepsPerRevolution;
  } else if (delta < -s4(ThreePhasePositionEstimator::StepsPerRevolution / 2)) {
    delta += ThreePhasePositionEstimator::StepsPerRevolution;
  }

  return delta;
}

s2 abs2(s2 num) {
  if (num < 0)
    return -num;
  return num;
}

void ServoController::init() {
  ThreePhaseController::init();

  servoMode = Mode::Init;

  onRotation = 0;

  position_P = 0;
  position_I = 0;
  position_D = 0; //(float) .12  = 61/512

  positionShift = 12;

  amplitudeCommand = 0;
  driveAmplitudeScaled = 0;

  // initialPhasePosition = ThreePhasePositionEstimator::getMagnetometerPhaseEstimate();
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
    static s2 lastCommand = 0;
    const s2 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s2 velocityDelta = vel - lastVel;

    const s2 velocityError = vel - velocityCommand;

    s4 command = lastCommand + ((velocityError * velocity_P + velocityDelta * velocity_D) >> velocityShift);

    constexpr s4 MAX = 255;
    if (command > MAX) {
      command = MAX;
    } else if (command < -MAX) {
      command = -MAX;
    }

    ThreePhaseController::setAmplitudeTarget(lastCommand = (s2)command);

    lastVel = vel;

  } else if (servoMode == Mode::Position) {

    // if(initialPhasePosition == 0) initialPhasePosition = ThreePhasePositionEstimator::getMagnetometerPhaseEstimate();

    const s2 vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    const s4 positionError = wrapdist(ThreePhasePositionEstimator::getMagnetometerPhaseEstimate(), positionCommand);

    s4 command = (-position_P * positionError >> 18) - (vel * position_D >> 6); // - vel * position_D;

    // s4 command = positionError / (1024 * 4);
    // s4 command = (positionError * position_P + vel * position_D / 512) >> positionShift;
    // s4 command = -vel * position_D / 512;

    constexpr s4 MAX = 255;
    if (command > MAX) {
      command = MAX;
    } else if (command < -MAX) {
      command = -MAX;
    }

    ThreePhaseController::setAmplitudeTarget((s2)command);

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

void ServoController::setPosition(u4 position) {
  servoMode = Mode::Position;
  positionCommand = position;
}

void ServoController::setDistance(s4 dist) {
  servoMode = Mode::Position;
  positionCommand += dist;
}

void ServoController::setEnable(bool enable) {
  if (!enable)
    servoMode = Mode::Init;
}
