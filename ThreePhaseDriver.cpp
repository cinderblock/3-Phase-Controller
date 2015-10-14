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

inline static void setUpdateLock(const bool lock) {
 /**
  * TCCR4E
  * TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0
  * 0b   0     1      0      0      0      0      0      0
  */
 TCCR4E = ((lock ? 1 : 0) << TLOCK4) | 0b01000000;
}

void ThreePhaseDriver::init() {
 // Turn off interrupts just in case
 TIMSK4 = 0;
 
 // Stop the timer just in case it's already running
 TCCR4B = 0;
 
 // Clear the high byte
 TC4H = 0;
 
 // Clear timer counter
 TCNT4 = 0;
 
 // Clear compare match registers for now
 OCR4A = 0;
 OCR4B = 0;
 OCR4D = 0;
 
 // Set high bits needed for TOP value for 11-bit PWM
 TC4H = 0b11;
 
 // Set the timer's TOP value
 OCR4C = 0xff;
 
 // Clear all interrupts just in case
 TIFR4 = 0xff;
 
 /**
  * DT4:
  * DT4H3 DT4H2 DT4H1 DT4H0 DT4L3 DT4L2 DT4L1 DT4L0
  * 0b  0     0     0     0     0     0     0     0
  */
 DT4 = 0;
 
 /**
  * TCCR4A:
  * COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
  * 0b   0      1      0      1     0     0     1     1
  */
 TCCR4A = 0b01010011;

 /**
  * TCCR4C
  * COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
  * 0b    0       0       0       0      0      1     0     1
  */
 TCCR4C = 0b00000101;

 /**
  * TCCR4D
  * FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
  * 0b  0     0     0     0     0    1     0     1
  */
 TCCR4D = 0b00000101;
 
 setUpdateLock(false);

 /**
  * TCCR4B
  * PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
  * 0b  1    1      0      0    0    0    0    1
  */
 TCCR4B = 0b11000001;
 
 // Turn everything off
 Board::DRV::AL.off();
 Board::DRV::BL.off();
 Board::DRV::CL.off();
 Board::DRV::AH.off();
 Board::DRV::BH.off();
 Board::DRV::CH.off();
 
 // Enable outputs
 Board::DRV::AL.output();
 Board::DRV::BL.output();
 Board::DRV::CL.output();
 Board::DRV::AH.output();
 Board::DRV::BH.output();
 Board::DRV::CH.output();
}

static const u2 limitedSinTable[ThreePhaseDriver::StepsPerPhase] PROGMEM = {
   0,  2,  4,  6,  8, 10, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31,
  33, 35, 37, 39, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64,
  66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96,
  98,100,101,103,105,107,109,111,113,115,117,118,120,122,124,126,
 128,129,131,133,135,136,138,140,142,143,145,147,149,150,152,154,
 155,157,159,160,162,163,165,167,168,170,171,173,174,176,177,179,
 180,182,183,185,186,188,189,190,192,193,194,196,197,198,200,201,
 202,204,205,206,207,208,210,211,212,213,214,215,217,218,219,220,
 221,222,223,224,225,226,227,228,229,230,231,231,232,233,234,235,
 236,236,237,238,239,239,240,241,241,242,243,243,244,245,245,246,
 246,247,247,248,248,249,249,250,250,250,251,251,252,252,252,253,
 253,253,253,254,254,254,254,254,254,255,255,255,255,255,255,255,
 255,255,255,255,255,255,255,255,254,254,254,254,254,254,253,253,
 253,253,252,252,252,251,251,250,250,250,249,249,248,248,247,247,
 246,246,245,245,244,243,243,242,241,241,240,239,239,238,237,236,
 236,235,234,233,232,231,231,230,229,228,227,226,225,224,223,222,
};

u1 ThreePhaseDriver::amplitude = 0;

u2 ThreePhaseDriver::getPhasePWM(const u1 step) {
 // u1 const sin = MAX * SIN(2 * PI * step / StepsPerCycle);
 u2 const sin = pgm_read_word(&limitedSinTable[step]);
// u2 const sin = step;
 
 // TODO: This product (and subsequent truncation) does not fully cover the
 // range of the return u1. Ideally, instead of dividing by 256 (>> 8) we should
 // be dividing by 255. We can get closer, on average, to that ideal division
 // if we add 188 for instance. Here we add both values that are bing multiplied
 // as a slightly better approximation at the extremes.
 u4 const prod = (u4)sin * amplitude + sin + amplitude;
 
 // Ideal:
// return prod / 255;
 
 // Close enough:
 return prod >> 8;
}

void ThreePhaseDriver::advance() {
 static u2 step = 0;
 advanceTo(step);
 Debug::reportByte(step >> 2);
 if (++step == 0x300) step = 0;
}

ThreePhaseDriver::Phase ThreePhaseDriver::currentPhase = Phase::INIT;

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
 OCR4D = val;
}

void ThreePhaseDriver::advanceToFullSine(const Phase phase, const u1 step) {
 u1 const ONE = getPhasePWM(    step);
 u1 const TWO = getPhasePWM(255-step);
 u1 const max = ONE > TWO ? ONE : TWO;
 
 setUpdateLock(true);
 
 if (phase == Phase::A) {
  setCompareMatchA(0);
  setCompareMatchB(TWO);
  setCompareMatchC(ONE);
 } else if (phase == Phase::B) {
  setCompareMatchA(ONE);
  setCompareMatchB(0);
  setCompareMatchC(TWO);
 } else if (phase == Phase::C) {
  setCompareMatchA(TWO);
  setCompareMatchB(ONE);
  setCompareMatchC(0);
 } else {
  // Should not get here. bad phase...
  
  setCompareMatchA(0);
  setCompareMatchB(0);
  setCompareMatchC(0);
  return;
 }
 
 setUpdateLock(false);

 // Save current phase
 currentPhase = phase;
}
