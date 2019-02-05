/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include "Board.h"
#include <avr/wdt.h>
#include <util/delay.h>

#include "Calibration.h"
#include "Clock.h"
#include "Debug.h"
#include "Demo.h"
#include "MLX90363.h"
#include "MLXDebug.h"
#include "SerialInterface.h"
#include "ServoController.h"
#include "ThreePhase/Controller.h"
#include "ThreePhase/FaultTester.h"
#include "USB/Descriptors.h"
#include "main.h"
#include <AVR++/FlashData.h>
#include <LUFA/Drivers/USB/USB.h>

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
  Debug::dout << PSTR("Beginning Inits\r\n");

  // Set up the driver pins
  ThreePhaseDriver::init();

  // Save and Clear the MCU Status Register. Indicates previous reset's source.
  resetCause = MCUSR;
  MCUSR = 0;

  ThreePhaseDriver::init();

  USB_Init();

  // Set Enable Interrupts.
  sei();

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();
}

State ThreePhaseControllerNamespace::state = State::Fault;
Fault ThreePhaseControllerNamespace::fault = Fault::Init;

bool ThreePhaseControllerNamespace::setState(State const s) {

  // Don't do anything if we're already in that mode, or trying to go to an invalid mode...
  if (state == s)
    return false;

  // Disable Interrupts
  cli();

  switch (s) {
  case State::Fault:
    ThreePhaseController::disable();
    ServoController::setEnable(false);
    break;

  case State::Calibration:
    ThreePhaseController::disable();
    ServoController::setEnable(false);
    break;

  case State::Push:
    ThreePhaseController::enable();
    ServoController::setEnable(false);
    break;
  case State::Servo:
    ThreePhaseController::enable();
    break;
  }

  state = s;

  // Enable Interrupts
  cli();

  return true;
}

/**
 *
 */
int main() {
  //  constexpr auto test = ThreePhaseFaultTester::Channel::AH;
  ////  constexpr auto test = ThreePhaseFaultTester::Phase::C;
  //  ThreePhaseFaultTester::init();
  //  ThreePhaseFaultTester::test(test);
  //  while (1);

  // These don't do anything if they're not enabled
  Demo::main();
  MLXDebug::main();

  SerialInterface::init();
  ThreePhaseController::init();

  while (1) {
    // These are very cheap calls if USB is not connected
    HID_Device_USBTask(&Generic_HID_Interface);
    USB_USBTask();

    // if (auto msg = SerialInterface::getMessage()) {
    //   ThreePhaseController::setAmplitudeTarget(msg->getCommand());
    // }

    switch (state) {
    case State::Fault:
      break;
    case State::Calibration:
      // TODO: Check for fault?
      break;
    case State::Push:
      // TODO: Check for fault?
      break;
    case State::Servo:
      ServoController::update();
    }
  }

  // loop in case main loop is disabled
  // allows for interrupts to continue
  while (1)
    continue;
}
