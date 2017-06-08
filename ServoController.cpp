
#include "ServoController.h"
#include "ThreePhaseController.h"
#include "DriverConstants.h"
// #include "common.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

ServoController::Mode ServoController::currentMode;

s2 ServoController::amplitudeCommand;
s4 ServoController::driveAmplitudeScaled;
s2 ServoController::velocityCommand;
s4 ServoController::distanceCommand;

u2 ServoController::velocityAdjust;

s4 ServoController::positionCommand;
s4 ServoController::shiftingLimit;
s2 ServoController::onRotation;

u2 ServoController::lastPosition;

u1 ServoController::P;
u1 ServoController::I;
u1 ServoController::D;

u1 ServoController::shift;

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

  lastPosition = MLX90363::getAlpha();

  currentMode = Mode::Init;

  onRotation = 0;

  P = 200;
  I = 0;
  D = 0;

  shift = 12;

  //256 is a velocity change of 1 per update
  velocityAdjust = 20;

  shiftingLimit = (ThreePhaseDriver::maxAmplitude << DriverConstants::drivePhaseValueShift) - velocityAdjust;

  amplitudeCommand = 0;
  driveAmplitudeScaled = 0;
}

void ServoController::update() {
  // This function is called every controlLoop(). It looks at its immediately available
  // data and calculates a signed amplitude to pass back to the ThreePhaseController.

  //figure out new current position
  const s2 pos = (s2)MLX90363::getAlpha();
  s2 delta = (s2)pos - (s2)lastPosition;
  lastPosition = pos;

  if (delta > (s2)(DriverConstants::MagnetometerMax / 2)) {
    onRotation++;
  } else if (delta < -((s2)DriverConstants::MagnetometerMax / 2)) {
    onRotation--;
  }

  if (currentMode == Mode::Init) {
    // DO NOTHING
  } else if (currentMode == Mode::Amplitude) {
    ThreePhaseController::setAmplitude(amplitudeCommand);

  } else if (currentMode == Mode::Velocity) {
    const s2 vel = ThreePhasePositionEstimator::getVelocity();

    s2 delta = vel - velocityCommand;

    if ((delta < 0 ? -delta : delta) > DeadBand) {
      if (delta < 0) {
        if (driveAmplitudeScaled < shiftingLimit) {
          driveAmplitudeScaled += velocityAdjust;
        } else {
          driveAmplitudeScaled = (ThreePhaseDriver::maxAmplitude << DriverConstants::drivePhaseValueShift);
        }
      } else {
        if (driveAmplitudeScaled > -(s2)shiftingLimit) {
          driveAmplitudeScaled -= velocityAdjust;
        } else {
          driveAmplitudeScaled = -(ThreePhaseDriver::maxAmplitude << DriverConstants::drivePhaseValueShift);
        }
      }
    }

    ThreePhaseController::setAmplitude((s2)(driveAmplitudeScaled >> DriverConstants::drivePhaseValueShift));

  } else if (currentMode == Mode::Position) {
//    const s2 vel = ThreePhasePositionEstimator::getVelocity();

    s4 distance = positionCommand - getPosition();

    s4 command = (distance * (s2)P); // + (vel * (s2)D);

    const s4 MAX = ((s4)ThreePhaseDriver::maxAmplitude) << shift;
    if (command > MAX) {
      command = MAX;
    } else if (command < -MAX) {
      command = -MAX;
    }

    ThreePhaseController::setAmplitude((s2)(command >> shift));
  } else if (currentMode == Mode::Distance) {
    // s2 pos = ThreePhaseController::getMeasuredPosition();
    // s2 vel = ThreePhaseController::getVelocity();

    // u1 command = dist(distanceCommand, pos, DriverConstants::StepsPerRotation) * P + vel * D;

    // if (command > ThreePhaseController::MaxAmplitude)
    //   command = ThreePhaseController::MaxAmplitude;
    // else if (command < -ThreePhaseController::MaxAmplitude)
    //   command = -ThreePhaseController::MaxAmplitude;

    // ThreePhaseController::setAmplitude(command);
  } else if (currentMode == Mode::Phase) {
    // return;
  } else {

  }
}

void ServoController::setAmplitude(s2 amplitude) {
  currentMode = Mode::Amplitude;

  amplitudeCommand = amplitude;
}

void ServoController::setVelocity(s2 velocity) {
  currentMode = Mode::Velocity;

  velocityCommand = velocity;
}

void ServoController::setPosition(s4 position) {
  currentMode = Mode::Position;

  positionCommand = position;
}

void ServoController::setDistance(s4 dist) {
  currentMode = Mode::Position;

  positionCommand = getPosition() + dist;
}

void ServoController::setEnable(bool enable) {
  if (!enable)
    currentMode = Mode::Init;
}
