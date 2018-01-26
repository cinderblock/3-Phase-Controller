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

  // Set up the driver pins
  ThreePhaseDriver::init();

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set up the hall sensor interrupts
  HallWatcher::init();

  // Set Enable Interrupts.
  sei();

  // MLX90363::init();
  // MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
  //
  // auto magRoll = MLX90363::getRoll();
  //
  // do {
  //   MLX90363::startTransmitting();
  //   // Delay long enough to guarantee data is ready
  //   _delay_ms(2);
  //
  //   // Loop until we actually receive real data
  // } while (!MLX90363::hasNewData(magRoll));

  // Enable timer overflow interrupt to use for automatically reading MLX
  TIMSK4 = 1 << TOIE4;

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  Board::LED0::off(); // lED0 is used in debug routines.

  // End of init
  Debug::dout << PSTR("Hello World!!\r\n");
}

constexpr u1 cyclesPWMPerMLX = 40;

void TIMER4_OVF_vect() {
  // Debug Trigger
  Board::LED1::on();
  Board::LED1::off();



  return;
  //
  u1 static mlx = cyclesPWMPerMLX;

  // Automatically start MLX communications every few ticks
  if (!--mlx) {
    MLX90363::startTransmitting();
    mlx = cyclesPWMPerMLX;
  }
}

Clock::MicroTime nextTime;



void printHallStateAndNumberIfHallChanged(u2 number) {
  u1 static lastState = HallWatcher::getState();

  const u1 state = HallWatcher::getState();

  if (state == lastState) return;

  lastState = state;
  Debug::dout << PSTR("New Hall State: ") << state << ',' << number << '\r' << '\n';
}

/**
 *
 **/
int main() {

  while (1) {
    printHallStateIfChanged();
    HallWatcher::checkAndUpdate();

    Debug::dout << PSTR("Holding for button press \r\n");
    holdUntilButtonFallingEdge();

    doSinePulse(64);
  }


  //loop in case main loop is disabled
  //allows for interrupts to continue
  while (1);
}
