
#include "ServoController.h"

ServoController::Mode ServoController::currentMode;

s2 ServoController::torqueCommand;
s2 ServoController::velocityCommand;
s4 ServoController::positionCommand;

ServoController::P;
ServoController::I;
ServoController::D;

u1 ServoController::currentLimit;

void ServoController::init(){
	currentMode = Mode::INIT;


}

void ServoController::update(){
	
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


void ServoController::setP(u1 p){
	P = p;
}

void ServoController::setI(u1 i){
	I = i;
}

void ServoController::setD(u1 d){
	D = d;
}


void ServoController::setTorqueEnable(bool enable){

	if(!enable)
		currentMode = Mode::INIT;

}

