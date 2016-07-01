
#include "ServoController.h"
#include "ThreePhaseController.h"
#include "DriverConstants.h"
// #include "common.h"

using namespace AVR;

ServoController::Mode ServoController::currentMode;

s2 ServoController::amplitudeCommand;
s4 ServoController::driveAmplitudeScaled;
s2 ServoController::velocityCommand;

u2 ServoController::velocityAdjust;

s4 ServoController::positionCommand;
s4 ServoController::shiftingLimit;

u1 ServoController::P;
u1 ServoController::I;
u1 ServoController::D;

u1 ServoController::currentLimit;

// distance function with a wrap around

s2 dist(u2 to, u2 from, u2 wrapdist) {

  s2 delta = (s2)to - (s2)from;

  if (delta > (s2)(wrapdist / 2)) {
    delta = ((s2)wrapdist) - delta;
  } else if (delta < -((s2)wrapdist / 2)) {
    delta = ((s2)wrapdist) + delta;
  }

  return delta;

}

s2 abs2(s2 num) {
  if (num < 0) return -num;
  return num;
}

const u1 amplitudeLimit = ThreePhaseController::MaxAmplitude;

void ServoController::init() {
  currentMode = Mode::Init;

  P = 8;
  I = 0;
  D = 0;

  //256 is a velocity change of 1 per update
  velocityAdjust = 20;

  shiftingLimit = (amplitudeLimit << DriverConstants::drivePhaseValueShift) - velocityAdjust;

  amplitudeCommand = 0;
  driveAmplitudeScaled = 0;
}

void ServoController::update() {

  if (currentMode == Mode::Init) {
    //DO NOTHING
  } else if (currentMode == Mode::Amplitude) {
    ThreePhaseController::setAmplitude(amplitudeCommand);

  } else if (currentMode == Mode::Velocity) {
    s2 vel = ThreePhaseController::getVelocity();

    s2 delta = vel - velocityCommand;

    if ((delta < 0 ? -delta : delta) > DeadBand) {
      if (delta < 0) {
	if (driveAmplitudeScaled < shiftingLimit)
	  driveAmplitudeScaled += velocityAdjust;
	else
	  driveAmplitudeScaled = (amplitudeLimit << DriverConstants::drivePhaseValueShift);
      } else {
	if (driveAmplitudeScaled > -(s2)shiftingLimit)
	  driveAmplitudeScaled -= velocityAdjust;
	else
	  driveAmplitudeScaled = -(amplitudeLimit << DriverConstants::drivePhaseValueShift);
      }
    }

    ThreePhaseController::setAmplitude((s2)(driveAmplitudeScaled >> DriverConstants::drivePhaseValueShift));

  } else if (currentMode == Mode::Position) {
    s2 pos = ThreePhaseController::getMeasuredPosition();
    s2 vel = ThreePhaseController::getVelocity();

    u1 command = dist(positionCommand, pos, DriverConstants::StepsPerRotation) * P + vel * D;

    ThreePhaseController::setAmplitude(command);
  } else {

  }

  ThreePhaseController::updateDriver();


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

void ServoController::setCurrentLimit(u1 current) {
  currentLimit = current;

}

void ServoController::setEnable(bool enable) {

  if (!enable)
    currentMode = Mode::Init;



}

