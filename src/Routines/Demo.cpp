/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Demo.cpp
 * Author: Cameron
 *
 * Created on April 11, 2017, 6:32 PM
 */

#include "Demo.hpp"
#include "../CalibrationData/LookupTable.hpp"
#include "../Sensors/MLX90363.hpp"
#include "../ThreePhase/Controller.hpp"
#include "../ThreePhase/Driver.hpp"
#include <AVR++/TimerTimeout.hpp>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <util/delay.h>

using namespace ThreePhaseControllerNamespace;

/**
 *
 */
void TIMER0_COMPA_vect() { Demo::timeout(); }

void Demo::main() {
  if (!enabled)
    return;

  _delay_ms(500);

  u1 mode = eeprom_read_byte(modeLocation) + 1;

  if (mode > modesMax)
    mode = 0;

  eeprom_update_byte(modeLocation, mode);

  //  if (mode == 0) dumbSpin::main();
  if (mode == 0)
    ManualConstantTorque::main();
  if (mode == 1)
    ManualConstantTorque::main(50);
  if (mode == 2)
    ManualConstantTorque::main(100);

  while (1)
    ;
}

void Demo::timeout() {
  if (timeoutFunc)
    timeoutFunc();
}

void (*Demo::timeoutFunc)() = nullptr;

void Demo::setTimeoutFunc(void (*tf)()) {
  ATOMIC_BLOCK(ATOMIC_FORCEON) { timeoutFunc = tf; }
}

bool Demo::dumbSpin::go = false;

void Demo::dumbSpin::main() {
  ThreePhaseDriver::PhasePosition p;

  ThreePhaseDriver::setAmplitude(10);

  setTimeoutFunc(&timeout);

  // Get things started
  timeout();

  constexpr u1 shift = 10;
  static_assert(shift < MLX90363::resolutionBits, "Need at least one bit of data from MLX");
  constexpr u2 iMax = (1 << (MLX90363::resolutionBits - shift)) - 2;
  u2 i = iMax;

  while (1) {
    const u2 alpha = MLX90363::getAlpha() >> shift;
    while (!go) {
      if (i > iMax)
        i = iMax;
      do {
        //        Board::LED::set(i < alpha);
      } while (i-- && !go);
    }
    go = false;
    ThreePhaseDriver::advanceTo(++p);
  }
}

void Demo::dumbSpin::timeout() {
  constexpr TimerTimeout::Period delayPeriod((long double)2.0 / 0x300);
  TimerTimeout::startA(delayPeriod);
  go = true;
}

void Demo::ManualConstantTorque::main(u1 ampl) {
  ThreePhaseController::init();
  ThreePhaseController::setAmplitudeTarget(ampl);
  while (1)
    ;
}

void Demo::PositionHold::main() {

  while (1) {
  }
}
