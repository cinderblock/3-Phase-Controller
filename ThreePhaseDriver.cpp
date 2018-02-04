/*
 * File:   ThreePhaseDriver.cpp
 * Author: Cameron
 *
 * Created on December 17, 2014, 3:20 PM
 */

#include <avr/pgmspace.h>

#include "ThreePhaseDriver.h"
#include "Board.h"
#include "Debug.h"
#include "AVRClock.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

inline static void setUpdateLock(const bool lock) {
  /**
   * TCCR4E
   * TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0
   * 0b   0     1      0      0      0      0      0      0
   */
  TCCR4E = ((lock ? 1 : 0) << TLOCK4) | 0b01000000;
}

void ThreePhaseDriver::init() {
  AVR::Clock::enablePLL();
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

  // Clear all interrupts just in case
  TIFR4 = 0xff;

  /**
   * DT4:
   * DT4H3 DT4H2 DT4H1 DT4H0 DT4L3 DT4L2 DT4L1 DT4L0
   * 0b  0     0     0     0     0     0     0     0
   */
  DT4 = 0xFF;

  /**
   * TCCR4D
   * FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
   * 0b  0     0     0     0     0    1     0     1
   */
  TCCR4D = 0b00000101;

  /**
   * TCCR4A:
   * COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
   * 0b   0      1      0      1     0     0     1     1
   */
  TCCR4A = 0b01010011;

  /**
   * TCCR4C
   * COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
   * 0b    0       1       0       1      0      1     0     1
   */
  TCCR4C = 0b01010101;

  // TCCR4E
  setUpdateLock(false);

  // Clear compare match registers for now
  OCR4A = 0;
  OCR4B = 0;
  OCR4D = 0;

  // Set high bits needed for TOP value for 11-bit PWM
  TC4H = 0xff;

  // Set the timer's TOP value
  OCR4C = 0xff;

  // Start the timer

  /**
   * TCCR4B
   * PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
   * 0b  0    1      0      0    0    0    0    1
   * The CS4n bits set the clock divider and start the timer.
   * A value of "1" in CS4x equates to ~31kHz PWM frequency
   * A value of "3" (0b11) equates to ~7.8kHz PWM frequency
   */

  // 31kHz
  TCCR4B = 0b01000001;

  // 7.8kHz
  // TCCR4B = 0b01000011;

  // Turn everything off
  Board::DRV::AL::off();
  Board::DRV::BL::off();
  Board::DRV::CL::off();
  Board::DRV::AH::off();
  Board::DRV::BH::off();
  Board::DRV::CH::off();
}

/**
 * This table is used to generate the two PWM values needed to generate 3-phase
 * sine waves. See "3 Phase sin waves" document.
 */
static const u2 limitedSinTable[ThreePhaseDriver::StepsPerPhase] PROGMEM = {
	   0,  17,  33,  50,  67,  84, 100, 117, 134, 151, 167, 184, 201, 217, 234, 251,
	 267, 284, 300, 317, 333, 350, 366, 383, 399, 416, 432, 449, 465, 481, 497, 514,
	 530, 546, 562, 578, 594, 610, 626, 642, 658, 674, 690, 705, 721, 737, 752, 768,
	 783, 799, 814, 830, 845, 860, 875, 890, 905, 920, 935, 950, 965, 980, 994,1009,
	1023,1038,1052,1067,1081,1095,1109,1123,1137,1151,1165,1179,1192,1206,1219,1233,
	1246,1259,1273,1286,1299,1312,1324,1337,1350,1362,1375,1387,1399,1411,1424,1436,
	1447,1459,1471,1483,1494,1505,1517,1528,1539,1550,1561,1572,1582,1593,1603,1614,
	1624,1634,1644,1654,1664,1674,1683,1693,1702,1711,1720,1729,1738,1747,1756,1764,
	1773,1781,1789,1797,1805,1813,1821,1828,1836,1843,1850,1858,1865,1871,1878,1885,
	1891,1898,1904,1910,1916,1922,1927,1933,1938,1944,1949,1954,1959,1964,1968,1973,
	1977,1982,1986,1990,1994,1997,2001,2004,2008,2011,2014,2017,2020,2022,2025,2027,
	2029,2032,2034,2035,2037,2039,2040,2041,2043,2044,2045,2045,2046,2046,2047,2047,
	2047,2047,2047,2046,2046,2045,2045,2044,2043,2041,2040,2039,2037,2035,2034,2032,
	2029,2027,2025,2022,2020,2017,2014,2011,2008,2004,2001,1997,1994,1990,1986,1982,
	1977,1973,1968,1964,1959,1954,1949,1944,1938,1933,1927,1922,1916,1910,1904,1898,
	1891,1885,1878,1871,1865,1858,1850,1843,1836,1828,1821,1813,1805,1797,1789,1781,
};

u1 ThreePhaseDriver::amplitude = 0;

u2 ThreePhaseDriver::getPhasePWM(const u1 step) {
  // Make sure we get 0 out if amplitude is set to 0.
  if (!amplitude) return 0;

  // u1 const sin = MAXpwm * SIN(2 * PI * step / StepsPerCycle);
  u2 const sin = pgm_read_word(&limitedSinTable[step]);

  // Ideally, we'd use this math.
  //return (u4)sin * amplitude / 255;

  // However /255 is slow

  // Let's approximate that, but faster by about 20 instructions:
  return ((u4)sin * amplitude + sin + amplitude) >> 8;
}

inline static void setCompareMatchA(u2 const val) __attribute__((always_inline, hot));
inline static void setCompareMatchB(u2 const val) __attribute__((always_inline, hot));
inline static void setCompareMatchC(u2 const val) __attribute__((always_inline, hot));

inline static void setCompareMatchA(u2 const val) {
  TC4H = val >> 8;
  OCR4A = val;
}

inline static void setCompareMatchB(u2 const val) {
  TC4H = val >> 8;
  OCR4B = val;
}

inline static void setCompareMatchC(u2 const val) {
  TC4H = val >> 8;
  // For some reason, the TIMER4 hardware skips C...
  OCR4D = val;
}

void ThreePhaseDriver::advanceTo(const PhasePosition pp) {
  auto const step = pp.getPosition();
  auto const phase = pp.getPhase();

  u2 const ONE = getPhasePWM(step);
  u2 const TWO = getPhasePWM(255 - step);
  u2 const OFF = 0;

  setUpdateLock(true);

  if (phase == Phase::A) {
    setCompareMatchA(OFF);
    setCompareMatchB(TWO);
    setCompareMatchC(ONE);
  } else if (phase == Phase::B) {
    setCompareMatchA(ONE);
    setCompareMatchB(OFF);
    setCompareMatchC(TWO);
  } else if (phase == Phase::C) {
    setCompareMatchA(TWO);
    setCompareMatchB(ONE);
    setCompareMatchC(OFF);
  } else {
    // Should not get here. bad phase...

    setCompareMatchA(0);
    setCompareMatchB(0);
    setCompareMatchC(0);
    return;
  }

  setUpdateLock(false);
}
