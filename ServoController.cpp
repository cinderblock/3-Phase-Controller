
#include "ServoController.h"
#include "ThreePhaseController.h"
#include "DriverConstants.h"

using namespace AVR;

ServoController::Mode ServoController::currentMode;

s2 ServoController::torqueCommand;
s2 ServoController::velocityCommand;

u1 ServoController::velocityAdjust;

s4 ServoController::positionCommand;

u1 ServoController::P;
u1 ServoController::I;
u1 ServoController::D;

u1 ServoController::currentLimit;

//distance function with a wrap around
s2 dist(u2 to, u2 from, u2 wrapdist){

	s2 delta = (s2)to - (s2)from; 
	
	if (delta > (s2)(wrapdist/2)){
		delta = ((s2)wrapdist) - delta;
	}
	else if (delta < -((s2)wrapdist/2)){
		delta = ((s2)wrapdist) + delta;
	}

	return delta;

}

void ServoController::init(){
	currentMode = Mode::INIT;

	P = 8;
	I = 0;
	D = 0;

	velocityAdjust = 1;
	torqueCommand = 0;
}

void ServoController::update(){

	if(currentMode == Mode::INIT){
		//DO NOTHING
	}
	else if(currentMode == Mode::TOR){
		ThreePhaseController::setTorque(torqueCommand);

	}
	else if(currentMode == Mode::VEL){
		s2 vel = ThreePhaseController::getVelocity();

		if(vel < velocityCommand){
			if(torqueCommand < ThreePhaseDriver::maxAmplitude)
				torqueCommand += velocityAdjust;
		}
		else if(vel > velocityCommand){
			if(torqueCommand > -ThreePhaseDriver::maxAmplitude)
				torqueCommand -= velocityAdjust;
		}

		ThreePhaseController::setTorque(torqueCommand);

	}
	else if(currentMode == Mode::POS){
		s2 pos = ThreePhaseController::getMeasuredPosition();
		s2 vel = ThreePhaseController::getVelocity();

		u1 command = dist(positionCommand, pos, DriverConstants::StepsPerRotation) * P + vel * D;

		ThreePhaseController::setTorque(command);
	}

	ThreePhaseController::updateDriver();


}


void ServoController::setTorque(s2 torque){
	currentMode = Mode::TOR;

	torqueCommand = torque;
}

void ServoController::setVelocity(s2 velocity){
	currentMode = Mode::VEL;

	velocityCommand = velocity;
}

void ServoController::setPosition(s4 position){
	currentMode = Mode::POS;

	positionCommand = position;
}


void ServoController::setCurrentLimit(u1 current){
	currentLimit = current;

}


void ServoController::setTorqueEnable(bool enable){

	if(!enable)
		currentMode = Mode::INIT;



}

