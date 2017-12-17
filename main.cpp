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
#include "HallWatcher.h"

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

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set up the hall sensor interrupts
  HallWatcher::init();

  // Set Enable Interrupts.
  sei();

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  Board::LED0::on();

  Debug::dout << 12345 << PSTR("Hello World!!\r\n");
  // End of init
}

Clock::MicroTime nextTime;

/**
 *
 */
int main() {

  // displayHallState();


  signed int command = 10;


  Clock::readTime(nextTime);
  while (1) {

    nextTime += 1_ms;
    while (!nextTime.isInPast()) {

    }

    Debug::dout << HallWatcher::getState() << PSTR(" is hall state\r\n");
  }

  //loop in case main loop is disabled
  //allows for interrupts to continue
  while (1);
}
