
#include "Interpreter.h"
#include "Config.h"
#include "ThreePhaseController.h"

void Interpreter::interpretFromMaster(u1* incomingData){
	if (checkCRC(incomingData)){
		return;
	}

	if(incomingData[0] == 0x20){
		s2 torque = *((s2*)(incomingData+1));
		
		if(torque > ThreePhaseController::getMaxTorque())
			torque = ThreePhaseController::getMaxTorque();
		else if ( torque < -ThreePhaseController::getMaxTorque())
			torque = -ThreePhaseController::getMaxTorque();

		ThreePhaseController::setTorque(torque);
	}
}

void Interpreter::sendNormalDataToMaster(){

}

//check the message to check if it passes CRC check
bool Interpreter::checkCRC(u1 const * const mes){
	u1 sum = 0;

	for(u1 i = 0; i < Config::i2cBufferIncomingSize; i++)
		//sum = _crc8_ccitt_update(sum, mes[i]);
		sum += mes[i];

	return sum != 0xFF;
}

u1 Interpreter::getCRC(u1 const * const mes, u1 length){
	u1 sum = 0;
	
	for(u1 i = 0; i < length; i++)
		//sum = _crc8_ccitt_update(sum, mes[i]);
		sum += mes[i];

	return ~sum;
}
