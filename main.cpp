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
// #include "Debug.h"
#include "Clock.h"
#include "Interpreter.h"
#include "ServoController.h"
#include "Demo.h"
#include "Calibration.h"
#include "Debug.h"
#include "commutation.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

#include "mainHelper.inc"

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

  Debug::init();
  Debug::dout << PSTR("Beginning Inits \r\n");

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set Enable Interrupts.
  sei();

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  // End of init
  Debug::dout << PSTR("End of Inits \r\n");
}

/**
 *
 */
int main() {
  Debug::dout << "Hello world!\n";

  if (Calibration::enabled) {
    Calibration::main();
  }
  else
  if (Demo::enabled) {
    Demo::main();
  }
  else {
    // Init for hardware interface.
    ServoController::init();

    //main loop
    while (1) {
      // Let ServoController calculate new amplitude command
      ServoController::update();
      
      
    }
  }

  //loop in case main loop is disabled
  //allows for interrupts to continue
  while (1);
}
