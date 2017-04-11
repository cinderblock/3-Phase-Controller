/* 
 * File:   FilebotInterface.cpp
 * Author: Cameron
 * 
 * Created on April 10, 2017, 11:21 PM
 */

#include "FilebotInterface.h"
#include "FilebotInterface/TwillBotInterface.cpp"

using namespace ThreePhaseControllerNamespace;



void TWI_vect() {
	Debug::TwillBotInterface::ISR::enter();
	CommInterface::InterruptServiceRoutine();
	Debug::TwillBotInterface::ISR::exit();
}

void TIMER0_COMPB_vect() {
	CommInterface::timeout();
}

template class FileBotInterface::TwillBotInterface<Config::i2cSlaveAddress, Config::i2cBufferIncomingSize, Config::i2cBufferOutgoingSize, Config::i2cBufferIncomingBlocks, 3>;