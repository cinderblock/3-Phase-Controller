/*
 * This version of the Three Phase Controller Software is a patch on the current
 * system to avoid floating motor coils.
 * works at 4kHz (instead of 32 kHz)
 *
 * This version is primarily for testing
 */


#include <avr/pgmspace.h>

#include "ThreePhaseDriver.h"
#include "Board.h"

// IOpin should be made optimized enough so we don't need to do this. Oh well.

inline static void ALowOff() {PORTB &= ~(1 << 4);}
inline static void BLowOff() {PORTF &= ~(1 << 0);}
inline static void CLowOff() {PORTB &= ~(1 << 0);}

inline static void ALowOn()  {PORTB |=  (1 << 4);}
inline static void BLowOn()  {PORTF |=  (1 << 0);}
inline static void CLowOn()  {PORTB |=  (1 << 0);}

inline static void AHighOff() {PORTB &= ~(1 << 5);}
inline static void BHighOff() {PORTB &= ~(1 << 6);}
inline static void CHighOff() {PORTB &= ~(1 << 7);}

inline static void AHighOn()  {PORTB |=  (1 << 5);}
inline static void BHighOn()  {PORTB |=  (1 << 6);}
inline static void CHighOn()  {PORTB |=  (1 << 7);}

inline static bool isAHighOn()  {return PORTB & (1 << 5);}
inline static bool isBHighOn()  {return PORTB & (1 << 6);}
inline static bool isCHighOn()  {return PORTB & (1 << 7);}

// Reserved AVR registers for timer operation so we don't need to waste time
// with context switching and such.

/**
 * Cached value to write to OCR1AL at the next overflow
 */
register u1 cachedOCRA asm("r2");

/**
 * Cached value to write to OCR1BL at the next overflow
 */
register u1 cachedOCRB asm("r3");

/**
 * Cached value to write to OCR1CL at the next overflow
 */
register u1 cachedOCRC asm("r4");

/**
 * Cached value to write to TIMSK1 at the next overflow.
 *
 * Used to select which interrupt will fire next
 */
register u1 cachedNextIntEnable asm("r5");

void ThreePhaseDriver::init() {
 TIMSK1 = 0;

 TIFR1 = 0b00101111;

 OCR1A = 0;
 OCR1B = 0;
 OCR1C = 0;

 /*
  * Set WGM for PWM Phase Correct 8-bit
  * WGM = 0b0001
  *
  * Eventually, set all compare output modules to:
  * Clear OC1[ABC] on compare match when up-counting.
  * Set OC1[ABC] on compare match when down-counting.
  * COM1[ABC] = 0b10
  *
  * This makes the OCR1[ABC] registers reflect the output's "high time" per cycle
  *
  * But for now, we leave them off and let the timed sequenced turn them on as needed
  *
  * Select the fastest clock divider ( clkIO/1 )
  *
  * CS1 = 0b001;
  */

 /**
  * TCCR1A:
  * COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
  * 0b   0      0      0      0      0      0     0     1
  */

 TCCR1A = 0b00000001;

 /**
  * TCCR1B
  * ICNC1 ICES1 ~~~~~ WGM13 WGM12 CS12 CS11 CS10
  * 0b  0     0     0     0     0    0    1    0
  */
 TCCR1B = 0b00000010;

 // Turn highs off
 Board::DRV::AH.off();
 Board::DRV::BH.off();
 Board::DRV::CH.off();
 
 // Enable outputs
 Board::DRV::AH.output();
 Board::DRV::BH.output();
 Board::DRV::CH.output();
 Board::DRV::AL.output();
 Board::DRV::BL.output();
 Board::DRV::CL.output();
 
 // Turn lows on
 Board::DRV::AL.on();
 Board::DRV::BL.on();
 Board::DRV::CL.on();

}

extern "C" void TIMER1_COMPA_vect() __attribute__((naked, __INTR_ATTRS));

extern "C" void TIMER1_COMPB_vect() __attribute__((naked, __INTR_ATTRS));

extern "C" void TIMER1_COMPC_vect() __attribute__((naked, __INTR_ATTRS));

inline static void delay() {
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
    asm("nop"::);
}

void TIMER1_COMPA_vect() {
 if (isAHighOn()) {
  AHighOff();
  delay();
  ALowOn();
  asm("reti" : :);
 }
 ALowOff();
 delay();
 AHighOn();
 asm("reti" : :);
}

void TIMER1_COMPB_vect() {
 if (isBHighOn()) {
  BHighOff();
  delay();
  BLowOn();
  asm("reti" : :);
 }
 BLowOff();
 delay();
 BHighOn();
 asm("reti" : :);
}

void TIMER1_COMPC_vect() {
 if (isCHighOn()) {
  CHighOff();
  delay();
  CLowOn();
  asm("reti" : :);
 }
 CLowOff();
 delay();
 CHighOn();
 asm("reti" : :);
}

extern "C" void TIMER1_OVF_vect() __attribute__((naked, __INTR_ATTRS));

void TIMER1_OVF_vect() {
 // clear timer match interrupts
 TIFR1 = cachedNextIntEnable;

 OCR1AL = cachedOCRA;
 OCR1BL = cachedOCRB;
 OCR1CL = cachedOCRC;

 // Enable next interrupts
 TIMSK1 = cachedNextIntEnable;

 asm("reti" : :);
}

constexpr u1 INTdefault = 0b00001110;

inline static void stageNextPhase(ThreePhaseDriver::Phase const phase) {
 if      (phase == ThreePhaseDriver::Phase::A) cachedNextIntEnable = INTdefault & ~(1 << OCIE1A);
 else if (phase == ThreePhaseDriver::Phase::B) cachedNextIntEnable = INTdefault & ~(1 << OCIE1B);
 else if (phase == ThreePhaseDriver::Phase::C) cachedNextIntEnable = INTdefault & ~(1 << OCIE1C);
}

inline static u1 max(u1 const a, u1 const b) {return a > b ? a : b;}
inline static u1 min(u1 const a, u1 const b) {return a < b ? a : b;}

constexpr u1 MIN = 5;
constexpr u1 MAX = 200;

void ThreePhaseDriver::advanceToFullSine(const Phase phase, const u1 step) {

 if (TIMSK1 & 1) return;

 u1 const ONE = min(getPhasePWM(      step), MAX);
 u1 const TWO = min(getPhasePWM(255 - step), MAX);

 if (phase == Phase::A) {
  stageNextPhase(Phase::A);
  cachedOCRA = 0;
  if (TWO < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1B);
   cachedOCRB = 0;
  } else {
   cachedOCRB = TWO;
  }
  if (ONE < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1C);
   cachedOCRC = 0;
  } else {
   cachedOCRC = ONE;
  }
 } else if (phase == Phase::B) {
  stageNextPhase(Phase::B);
  cachedOCRB = 0;
  if (TWO < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1C);
   cachedOCRC = 0;
  } else {
   cachedOCRC = TWO;
  }
  if (ONE < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1A);
   cachedOCRA = 0;
  } else {
   cachedOCRA = ONE;
  }
 } else if (phase == Phase::C) {
  stageNextPhase(Phase::C);
  cachedOCRC = 0;
  if (TWO < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1A);
   cachedOCRA = 0;
  } else {
   cachedOCRA = TWO;
  }
  if (ONE < MIN) {
   cachedNextIntEnable &= ~(1 << OCIE1B);
   cachedOCRB = 0;
  } else {
   cachedOCRB = ONE;
  }
 }

 TIFR1 = 0b00000001;
 TIMSK1 = 0b00000001;

}

static const u1 limitedSinTable[ThreePhaseDriver::StepsPerPhase] PROGMEM = {
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

u1 ThreePhaseDriver::amplitude;

u1 ThreePhaseDriver::getPhasePWM(const u1 step) {
 u1 const sin = step;
// u1 const sin = pgm_read_byte(&limitedSinTable[step]);

 // TODO: This product (and subsequent truncation) does not fully cover the
 // range of the return u1. Ideally, instead of dividing by 256 (>> 8) we should
 // be dividing by 255. We can get closer, on average, to that ideal division
 // if we add 188 for instance. Here we add both values that are bing multiplied
 // as a slightly better approximation at the extremes.
 u2 const prod = sin * amplitude + sin + amplitude;

 // Ideal:
 // return prod / 255;

 // Close enough:
 return prod >> 8;
}
