/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include "Board.hpp"
#include <util/delay.h>

#include "Calibration.hpp"
#include "Clock.hpp"
#include "Debug.hpp"
#include "Demo.hpp"
#include "MLX90363.hpp"
#include "MLXDebug.hpp"
#include "SerialInterface.hpp"
#include "ServoController.hpp"
#include "ThreePhase/Controller.hpp"
#include "ThreePhase/FaultTester.hpp"
#include "USB/Descriptors.h"
#include "main.hpp"
#include <AVR++/FlashData.hpp>
#include <AVR++/WDT.hpp>
#include <LUFA/Drivers/USB/USB.h>

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

// Save reset cause
const u1 resetCause = MCUSR;

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
__attribute__((constructor)) void init() {
  // Watch Dog Timer
  wdt_reset();
  wdt_disable();

  Debug::init();
  Debug::dout << PSTR("Beginning Inits\r\n");

  // Clear the MCU Status Register so that we can distinguish reset reasons more easily.
  MCUSR = 0;

  // Set up the driver pins
  ThreePhaseDriver::init();
  MLX90363::init();

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
    ThreePhaseController::stop();
    ThreePhaseDriver::advanceTo(-1);
    ServoController::setEnable(false);
    break;

  case State::Manual:
    ServoController::setEnable(false);
    ThreePhaseController::stop();
    MLX90363::stopTransmitting();
    break;

  case State::Normal:
    ThreePhaseController::init();
    ThreePhaseController::enable();
    break;
  }

  state = s;

  // Enable Interrupts
  sei();

  return true;
}

volatile Atomic<u2> ADCValues::AS;
volatile Atomic<u2> ADCValues::BS;
volatile Atomic<u2> ADCValues::CS;
volatile Atomic<u2> ADCValues::current;
volatile Atomic<u2> ADCValues::currentRef;
volatile Atomic<u2> ADCValues::battery;
volatile Atomic<u2> ADCValues::drive;
volatile Atomic<u2> ADCValues::temperature;

void Analog::AS() { ADCValues::AS.setUnsafe(ADC); }

void Analog::BS() { ADCValues::BS.setUnsafe(ADC); }

void Analog::CS() { ADCValues::CS.setUnsafe(ADC); }

void Analog::battery() { ADCValues::battery.setUnsafe(ADC); }

void Analog::drive() {
  const auto adc = ADC;

  if (adc < 350)
    ThreePhaseDriver::emergencyDisable();

  ADCValues::drive.setUnsafe(adc);
}

void Analog::temperature() { ADCValues::temperature.setUnsafe(ADC); }

void Analog::current() { ADCValues::current.setUnsafe(ADC); }

void Analog::currentRef() { ADCValues::currentRef.setUnsafe(ADC); }

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

  WDT::start(WDT::T0120ms);

  while (1) {
    WDT::tick();
    // These are very cheap calls if USB is not connected
    HID_Device_USBTask(&Generic_HID_Interface);
    USB_USBTask();

    // if (auto msg = SerialInterface::getMessage()) {
    //   ThreePhaseController::setAmplitudeTarget(msg->getCommand());
    // }

    switch (state) {
    default:
    case State::Fault:
      break;
    case State::Manual:
      // TODO: Check for fault?
      break;
    case State::Normal:
      // TODO: Check for fault?
      ServoController::update();
      break;
    }
  }

  // loop in case main loop is disabled
  // allows for interrupts to continue
  while (1)
    continue;
}
