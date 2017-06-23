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
#include "FilebotInterface.h"
// #include "Debug.h"
#include "Clock.h"
#include "Interpreter.h"
#include "ServoController.h"
#include "Demo.h"
#include "Calibration.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

u1 resetCause = MCUSR;

void init() {
  // Watch Dog Timer
  wdt_reset();
  wdt_disable();


  // Debug::init();

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set Enable Interupts.
  sei();

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  // i2c interface init
  CommInterface::init();

  // Interpret i2c communication interface.
  Interpreter::Init();

  // Init for hardware interface.
  ServoController::init();

  // Turn off led.
  Board::LED::output();
  Board::LED::off();

  // End of init
}

/**
 *
 */
void main() {

  if (Demo::enabled) Demo::main();
  else if (Calibration::enabled) Calibration::main();
  else {
    ThreePhaseController::enable();

    //main loop
    while (1) {
      // Let ServoController calculate new amplitude command
      ServoController::update();

      //get any incoming communications
      u1 const * const buff = CommInterface::getIncomingReadBuffer();

      //if there is a communication interpret it
      if (buff) {
        //interpret the new communication
        Interpreter::interpretFromMaster(buff);

        //prepare for next communication
        CommInterface::reserveNextReadBuffer();
      }

      //send whatever data we have back to master
      Interpreter::sendNormalDataToMaster();

      //silly fix in case of an error state
      CommInterface::fixWriteBuffer();
    }
  }

  //loop in case main loop is disabled
  //allows for interrupts to continue
  while (1);
}
