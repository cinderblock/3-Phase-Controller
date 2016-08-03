
#include "ServoController.h"
#include "ThreePhaseController.h"
#include "DriverConstants.h"
// #include "common.h"

using namespace AVR;

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

u1 ServoController::currentLimit;

// distance function with a wrap around

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

  P = 10;
  I = 0;
  D = 0;

  shift = 14;

  //256 is a velocity change of 1 per update
  velocityAdjust = 20;

  shiftingLimit = (ThreePhaseController::getMaxAmplitude() << DriverConstants::drivePhaseValueShift) - velocityAdjust;

  amplitudeCommand = 0;
  driveAmplitudeScaled = 0;
}

void ServoController::update() {

	//figure out new current position
	const s2 pos = (s2) MLX90363::getAlpha();
	s2 delta = (s2)pos - (s2)lastPosition;
	lastPosition = pos;

	if (delta > (s2)(DriverConstants::MagnetometerMax / 2))
		ServoController::incrementRotation(true);
	else if (delta < -((s2)DriverConstants::MagnetometerMax / 2))
		ServoController::incrementRotation(false);


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
	  driveAmplitudeScaled = (ThreePhaseController::getMaxAmplitude() << DriverConstants::drivePhaseValueShift);
      } else {
	if (driveAmplitudeScaled > -(s2)shiftingLimit)
	  driveAmplitudeScaled -= velocityAdjust;
	else
	  driveAmplitudeScaled = -(ThreePhaseController::getMaxAmplitude() << DriverConstants::drivePhaseValueShift);
      }
    }

    ThreePhaseController::setAmplitude((s2)(driveAmplitudeScaled >> DriverConstants::drivePhaseValueShift));

  } else if (currentMode == Mode::Position) {
    const s2 vel = ThreePhaseController::getVelocity();

		s4 distance = 
			(positionCommand & (~((u4)DriverConstants::MagnetometerBitsMask))) -
			((s4)onRotation << DriverConstants::MagnetometerBits) +
			(positionCommand & DriverConstants::MagnetometerBitsMask) - 
			pos;

		s4 command = (distance) * P + (vel) * D;

    const static s4 MAX = ThreePhaseController::getMaxAmplitude() << shift;
    if (command > MAX)
      command = MAX;
    else if (command < -MAX)
      command = -MAX;

    ThreePhaseController::setAmplitude(command >> shift);
  } else if (currentMode == Mode::Distance) {
    // s2 pos = ThreePhaseController::getMeasuredPosition();
    // s2 vel = ThreePhaseController::getVelocity();

    // u1 command = dist(distanceCommand, pos, DriverConstants::StepsPerRotation) * P + vel * D;

    // if (command > ThreePhaseController::MaxAmplitude)
    //   command = ThreePhaseController::MaxAmplitude;
    // else if (command < -ThreePhaseController::MaxAmplitude)
    //   command = -ThreePhaseController::MaxAmplitude;

    // ThreePhaseController::setAmplitude(command);
  } else if(currentMode == Mode::Phase){
	  // return;
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

void ServoController::setDistance(s4 distance) {
  currentMode = Mode::Position;

  onRotation = 0;
  positionCommand = distance - ThreePhaseController::getPredictedPosition();
}

void ServoController::setCurrentLimit(u1 current) {
  currentLimit = current;
}

void ServoController::setEnable(bool enable) {
  if (!enable)
    currentMode = Mode::Init;
}

