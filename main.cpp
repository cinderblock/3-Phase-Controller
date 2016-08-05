/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include <util/delay.h>
#include <avr/wdt.h>
#include "Board.h"

#include "ThreePhaseController.h"
#include "MLX90363.h"
#include "FilebotInterface/TwillBotInterface.h"
// #include "Debug.h"
#include "Clock.h"
#include "Interpreter.h"
#include "ServoController.h"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

u1 resetCause = MCUSR;

void init() {
	wdt_reset();
	wdt_disable();
	// Debug::init();

	MCUSR = 0;
	
	//enable interupts
	sei();
	
	::Clock::init();
	
	//i2c interface init
	TwillBotInterface::init();

	//Interpret i2c communication interface
	Interpreter::Init();

	//Init for hardware interface
	ServoController::init();

	//turn off led
	Board::LED.output();
	Board::LED.off();

	// End of init
}

/**
 * 
 */
void main() {
	
	u2 pos = 0;
	
	Clock::MicroTime t(0);
	Clock::MicroTime delta = 25_ms;
	Clock::MicroTime now;


	//set to drive at 0 speed
	ThreePhaseController::setAmplitude(0);

	//main loop
	while(1){

		//update hardware
		ServoController::update();

		//get any incoming communications
		u1 const * const buff = TwillBotInterface::getIncomingReadBuffer();

		//if there is a communication interpret it
		if (buff) {
			//interpret the new communication
			Interpreter::interpretFromMaster(buff);

			//prepare for next communication
			TwillBotInterface::reserveNextReadBuffer();
		}

		//send whatever data we have back to master
		Interpreter::sendNormalDataToMaster();

		//silly fix in case of an error state
		TwillBotInterface::fixWriteBuffer();
	}

	//loop in case main loop is disabled
	//allows for interrupts to continue
	while(1);
}

