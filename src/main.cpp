/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include "Board.hpp"
#include <util/atomic.h>
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

// Save reset causes
const ResetCauses ThreePhaseControllerNamespace::resetCauses;

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
__attribute__((constructor)) void init() {
  // Watch Dog Timer
  wdt_reset();
  wdt_disable();

  // Check if we're doing it in the wrong order...
  if (resetCauses.byte != MCUSR)
    Board::LED::on();

  if (resetCauses.WatchDog) {
    Board::LED::on();
    fault = Fault::WatchdogReset;
  }

  if (resetCauses.BrownOut) {
    Board::LED::on();
    fault = Fault::BrownOutReset;
  }

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

void ThreePhaseControllerNamespace::clearFault() {
  if (!setState(State::Fault)) {
    fault = Fault::Init;
    ThreePhaseDriver::emergencyOK();
    Board::LED::off();
  }
}

bool ThreePhaseControllerNamespace::setState(State const s) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {

    // Don't do anything if we're already in that mode, or trying to go to an invalid mode...
    if (state == s)
      return false;

    switch (s) {
    case State::Fault:
      ThreePhaseDriver::emergencyDisable();
      ThreePhaseController::stop();
      Board::LED::off();
      ServoController::setEnable(false);
      break;

    case State::Manual:
      stopSynchronous();
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
    return true;
  }
  return true;
}

volatile Atomic<u2> ADCValues::AS;
volatile Atomic<u2> ADCValues::BS;
volatile Atomic<u2> ADCValues::CS;
volatile Atomic<s2> ADCValues::current;
volatile Atomic<u2> ADCValues::battery;
volatile Atomic<u2> ADCValues::drive;
volatile Atomic<u2> ADCValues::temperature;

void Analog::AS() { ADCValues::AS.setUnsafe(ADC); }

void Analog::BS() { ADCValues::BS.setUnsafe(ADC); }

void Analog::CS() { ADCValues::CS.setUnsafe(ADC); }

void Analog::battery() { ADCValues::battery.setUnsafe(ADC); }

void Analog::drive() {
  const auto adc = ADC;

  if (adc < 350) {
    ThreePhaseDriver::emergencyDisable();

    if (setState(State::Fault))
      fault = Fault::UndervoltageLockout;
  }

  ADCValues::drive.setUnsafe(adc);
}

void Analog::temperature() {
  const auto adc = ADC;

  if (adc > 400) {
    ThreePhaseDriver::emergencyDisable();

    if (setState(State::Fault))
      fault = Fault::OverTemperature;
  }

  ADCValues::temperature.setUnsafe(adc);
}

union ADCt {
  struct {
    s2 s : 10;
  };
  struct {
    u2 u : 10;
  };
  u2 raw;

  inline ADCt(u2 value) : raw(value) {}
};

void Analog::current() { ADCValues::current.setUnsafe(((ADCt)ADC).s); }

Atomic<u8> synchronousPosition;
Atomic<s4> synchronousVelocity;
Clock::MicroTime lastSyncTime;
enum class SynchronousMode { Off, Running, StartUp };

const u1 OverPrecisionBits = 32;

Atomic<SynchronousMode> syncMode = SynchronousMode::Off;

void synchronousUpdate() {
  if (syncMode == SynchronousMode::Off) {
    return;
  }

  Clock::MicroTime now;
  Clock::readTime(now);

  if (syncMode == SynchronousMode::Running) {
    auto deltaTime = now;
    deltaTime -= lastSyncTime;

    const auto delta = deltaTime.timerTicks() * synchronousVelocity;

    synchronousPosition += delta;

    const auto synchronousPositionLimit = u8(ThreePhaseDriver::StepsPerRevolution) << OverPrecisionBits;

    while (synchronousPosition > synchronousPositionLimit) {
      if (delta > 0)
        synchronousPosition -= synchronousPositionLimit;
      else
        synchronousPosition += synchronousPositionLimit;
    }

    ThreePhaseDriver::advanceTo(getManualPosition());
  } else {
    Clock::readTime(lastSyncTime);
    synchronousPosition = 0;
    syncMode = SynchronousMode::Running;
  }

  lastSyncTime = now;
}

ThreePhaseDriver::PhasePosition ThreePhaseControllerNamespace::getManualPosition() {
  if (syncMode == SynchronousMode::Running) {
    return u2(synchronousPosition >> OverPrecisionBits);
  }

  // TODO: get position during other manual modes
  return 0;
}

void ThreePhaseControllerNamespace::setSynchronous(s4 const velocity) {
  if (syncMode != SynchronousMode::Running) {
    syncMode = SynchronousMode::StartUp;
  }

  // Multiply velocity in counts/second by ((1 << OverPrecisionBits)/MicroTicksPerSecond) for correct units
  synchronousVelocity = velocity;
}

s4 ThreePhaseControllerNamespace::getSynchronous() {
  if (syncMode == SynchronousMode::Off)
    return 0;

  // Multiply velocity in counts/second by ((1 << OverPrecisionBits)/MicroTicksPerSecond) for correct units
  return synchronousVelocity;
}

void ThreePhaseControllerNamespace::stopSynchronous() {
  syncMode = SynchronousMode::Off;
  // ThreePhaseDriver::setAmplitude(0);
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

  while (1) {
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
      synchronousUpdate();
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

void ThreePhaseControllerNamespace::bootloaderJump() {
  WDT::stop();

  // Disable interrupts
  cli();

  // Shutdown important hardware
  ThreePhaseDriver::emergencyDisable();

  // WDT::start(WDT::T__15ms);
  // while (1)
  //   continue;
  /**
   * We might try to just make a WDT timeout happen to reset but,
   * because of a bug in the old version of the LUFA DFU bootloader that we're using,
   * that does not work for us. Since it does not, we use the following "hack".
   */

  // Extracted from a map of the current build for the bootloader (MagicBootKey)
  constexpr u2 BootloaderMagicKeyRamAddress = 0x186;
  // From bootloader source: MAGIC_BOOT_KEY
  constexpr u2 BootloaderMagicKeyValue = 0xDC42;
  // From our current bootloader size selection in AVR Fuses (BOOTSZ=00)
  constexpr u2 BootloaderResetWordAddress = 0x3800;

  // Bootloader expects a specific ram localtion set to a certain value
  *(volatile u2 *)(BootloaderMagicKeyRamAddress) = BootloaderMagicKeyValue;

  // Maybe this is the actual address...
  *(volatile u2 *)(0x180) = BootloaderMagicKeyValue;

  // Jump to the bootloader
  ((void (*)(void))BootloaderResetWordAddress)();
}