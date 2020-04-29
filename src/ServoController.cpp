
#include "ServoController.hpp"
#include "ThreePhase/Controller.hpp"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

#define STATIC(member) decltype(ServoController::member) ServoController::member

STATIC(servoMode) = Mode::Disabled;

STATIC(position) = 0;

STATIC(amplitudeCommand) = 0;
STATIC(velocityCommand) = 0;
STATIC(positionCommand) = 0;

STATIC(kPosition) = {0, 0, 0};
STATIC(kVelocity) = {0, 0, 0};

STATIC(limits) = {{0, 0}, {0, 0}, {0, 0}};

void ServoController::init() {
  ThreePhaseController::init();

  servoMode = Mode::Disabled;
}

void ServoController::update() {
  // This function is called as often as possible in the main loop.
  // It looks at its currently available data and calculates a signed amplitude
  // to pass back to the ThreePhaseController

  const auto vel = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();
  static auto lastVel = decltype(vel){0};

  position.update(ThreePhasePositionEstimator::getMagnetometerPhaseEstimate());

  if (servoMode == Mode::Disabled) {
    // DO NOTHING
  } else if (servoMode == Mode::Amplitude) {
    ThreePhaseController::setAmplitudeTarget(amplitudeCommand);

  } else if (servoMode == Mode::Velocity) {
    static ThreePhaseController::Amplitude lastCommand = 0;

    const s2 velocityDelta = vel - lastVel;

    const s2 velocityError = vel - velocityCommand;

    // TODO: See if this needs any byte shifting to keep sane ranges
    s4 command = lastCommand + ((velocityError * kVelocity.kP + velocityDelta * kVelocity.kD));

    lastCommand = command;

    ThreePhaseController::setAmplitudeTarget(command);

  } else if (servoMode == Mode::Position) {

    const auto positionError = position - positionCommand;

    ThreePhaseController::Amplitude command = ((-positionError * kPosition.kP) >> 16) - ((vel * kPosition.kD) >> 8);

    ThreePhaseController::setAmplitudeTarget(command);
  } else {
  }

  lastVel = vel;
}
