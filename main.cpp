/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include "Board.h"
#include <avr/wdt.h>
#include <util/delay.h>

#include "MLX90363.h"
#include "ThreePhaseController.h"
// #include "Debug.h"
#include "Calibration.h"
#include "Clock.h"
#include "Debug.h"
#include "Demo.h"
#include "Interpreter.h"
#include "ServoController.h"
#include "MLXDebug.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

// Save reset cause
u1 resetCause;

void init() {
  // Watch Dog Timer
  wdt_reset();
  wdt_disable();

  Debug::init();
//  Debug::dout << PSTR("Beginning Inits\r\n");

  // Save and Clear the MCU Status Register. Indicates previous reset's source.
  resetCause = MCUSR;
  MCUSR = 0;

  // Set Enable Interrupts.
  sei();

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  // End of init
//  Debug::dout << PSTR("End of Inits\r\n");
}

/**
 *
 */
int main() {
  // Each of these does nothing if they're not enabled
  MLXDebug::main();
  Calibration::main();
  Demo::main();
    
  
  // Init for hardware interface.
  ServoController::init();

  // main loop
  while (1) {
    // Let ServoController calculate new amplitude command
    ServoController::update();
  }

  // loop in case main loop is disabled
  // allows for interrupts to continue
  while (1);
}
