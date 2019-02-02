/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ThreePhaseFaultTester.cpp
 * Author: Cameron
 *
 * Created on April 16, 2018, 12:58 PM
 */

#include "ThreePhaseFaultTester.h"
#include "AVRClock.h"
#include "Board.h"
#include <avr/io.h>

using namespace ThreePhaseControllerNamespace;

void ThreePhaseFaultTester::init() {
  AVR::Clock::enablePLL();

  // Turn everything off
  Board::DRV::AL::off();
  Board::DRV::BL::off();
  Board::DRV::CL::off();
  Board::DRV::AH::off();
  Board::DRV::BH::off();
  Board::DRV::CH::off();

  AVR::Clock::waitForPLL();

  // Turn off all timer 4 interrupts just in case
  TIMSK4 = 0;

  // Setup the timer but stopped.
  /**
   * TCCR4B
   * PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
   * 0b  0    1      0      0    0    0    0    0
   */
  TCCR4B = 0b01000000;

  // Clear the high byte
  TC4H = 0;

  // Clear timer counter
  TCNT4 = 0;

  // Clear all interrupts
  TIFR4 = 0xff;

  // Set dead times
  /**
   * DT4:
   * DT4H3 DT4H2 DT4H1 DT4H0 DT4L3 DT4L2 DT4L1 DT4L0
   * 0b  0     0     0     0     0     0     0     0
   */
  DT4 = 0xFF;

  // Make sure fault protection is off, clear the possible flag, and set PWM mode
  /**
   * TCCR4D
   * FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
   * 0b  0     0     0     0     0    1     1     1
   */
  TCCR4D = 0b00000111;

  // Setup output configuration for A & B
  /**
   * TCCR4A:
   * COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
   * 0b   0      1      0      1     0     0     1     1
   */
  TCCR4A = 0b01010011;

  // Setup output configuration for C (aka D). Also repeat shadow values for A & B
  /**
   * TCCR4C
   * COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
   * 0b    0       1       0       1      0      1     0     1
   */
  TCCR4C = 0b01010101;

  TCCR4E = (0 << TLOCK4) | (1 << ENHC4);

  // Set high bits needed for TOP value for 11-bit PWM
  TC4H = 0xff;

  // Set the timer's TOP value
  OCR4C = 0xff;

  // Start the timer

  /**
   * TCCR4B
   * PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
   * 0b  0    1      0      0    0    0    0    1
   * PSR4 resets the internal prescaler to 0
   * The CS4n bits set the clock divider and start the timer.
   * A value of "1" in CS4x equates to ~31kHz PWM frequency
   * A value of "3" (0b11) equates to ~7.8kHz PWM frequency
   */

  // 31kHz
  TCCR4B = 0b01000001;

  // 7.8kHz
  // TCCR4B = 0b01000011;

  // Set all outputs to ~50%
  constexpr u2 val = 2047 / 2;

  TC4H = val >> 8;
  OCR4A = u1(val);
  OCR4B = u1(val);
  OCR4D = u1(val);

  while (!TIFR4)
    continue;
}

ThreePhaseFaultTester::FailMode ThreePhaseFaultTester::test(Phase p) {
  switch (p) {
  case Phase::A:
    TCCR4E = 0b01000000 | (1 << OC4OE0) | (1 << OC4OE1);
    break;
  case Phase::B:
    TCCR4E = 0b01000000 | (1 << OC4OE2) | (1 << OC4OE3);
    break;
  case Phase::C:
    TCCR4E = 0b01000000 | (1 << OC4OE4) | (1 << OC4OE5);
    break;

  default:
    break;
  }

  return FailMode::Fail;
}

ThreePhaseFaultTester::FailMode ThreePhaseFaultTester::test(Channel c) {
  switch (c) {
  case Channel::AL:
    TCCR4E = 0b01000000 | (1 << OC4OE0);
    break;
  case Channel::AH:
    TCCR4E = 0b01000000 | (1 << OC4OE1);
    break;
  case Channel::BL:
    TCCR4E = 0b01000000 | (1 << OC4OE2);
    break;
  case Channel::BH:
    TCCR4E = 0b01000000 | (1 << OC4OE3);
    break;
  case Channel::CL:
    TCCR4E = 0b01000000 | (1 << OC4OE4);
    break;
  case Channel::CH:
    TCCR4E = 0b01000000 | (1 << OC4OE5);
    break;
  }

  return FailMode::Fail;
}
