/*
 * This version of the Three Phase Controller Software is a patch on the current
 * system to avoid floating motor coils.
 * works at 4kHz (instead of 32 kHz)
 * 
 * This version is primarily for testing
 */


#include "ThreePhaseDriver.h"

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

// Reserved AVR registers for timer operation so we don't need to waste time
// with context switching and such.

/**
 * Next value to write to TCCR1A in each OC interrupt.
 * 
 * Used for disabling high side drivers when using their compare match interrupt.
 */
register u1 cachedTgl asm("r2");

/**
 * Cached value to write to OCR1AL at the next overflow
 */
register u1 cachedOCRA asm("r3");

/**
 * Cached value to write to OCR1BL at the next overflow
 */
register u1 cachedOCRB asm("r4");

/**
 * Cached value to write to OCR1CL at the next overflow
 */
register u1 cachedOCRC asm("r5");

/**
 * Cached value to write to TIMSK1 at the next overflow.
 * 
 * Used to select which interrupt will fire next
 */
register u1 cachedNextIntEnable asm("r6");


void ThreePhaseDriver::init(){
 TIMSK1 = 0b00001111;
 
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
  * 0b  0     0     0     0     0    0    0    1
  */
 TCCR1B = 0b00000010;
 
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

enum class StepLocation : u1 {INIT=0, up1=1, up2=2, down1=3, down2=4 };

extern "C" void TIMER1_COMPA_vect() __attribute__ ((naked,__INTR_ATTRS));

extern "C" void TIMER1_COMPB_vect() __attribute__ ((naked,__INTR_ATTRS));

extern "C" void TIMER1_COMPC_vect() __attribute__ ((naked,__INTR_ATTRS));

void TIMER1_COMPA_vect(){
    if(cachedTgl == StepLocation::up1 || cachedTgl == StepLocation::up2){
        ALowOff();
        cachedTgl++;
        AHighOn();
    }
    else{
        AHighOff();
        if (cachedTgl == StepLocation::down1) cachedTgl++;
        else cachedTgl = StepLocation::up1;
        AHighOn();
    }
    
    asm("reti" : : );
}

void TIMER1_COMPB_vect(){
    if(cachedTgl == StepLocation::up1 || cachedTgl == StepLocation::up2){
        ALowOff();
        cachedTgl++;
        AHighOn();
    }
    else{
        BHighOff();
        if (cachedTgl == StepLocation::down1) cachedTgl++;
        else cachedTgl = StepLocation::up1;
        BHighOn();
    }
    
    
    asm("reti" : : );
}

void TIMER1_COMPC_vect(){
    if(cachedTgl == StepLocation::up1 || cachedTgl == StepLocation::up2){
        ALowOff();
        cachedTgl++;
        AHighOn();
    }
    else{
        BHighOff();
        if (cachedTgl == StepLocation::down1) cachedTgl++;
        else cachedTgl = StepLocation::up1;
        BHighOn();
    }
    
    asm("reti" : : );
}

extern "C" void TIMER1_OVF_vect() __attribute__ ((naked,__INTR_ATTRS));

void TIMER1_OVF_vect() {
 // clear timer match interrupts
 TIFR1 = cachedNextIntEnable; 
    
 OCR1AL = cachedOCRA;
 OCR1BL = cachedOCRB;
 OCR1CL = cachedOCRC;
 
 // Enable next interrupt
 TIMSK1 = cachedNextIntEnable;
 
 asm("reti" : : );
}


void ThreePhaseDriver::advanceToFullSine(const Phase phase, const u1 step) {

 u1 const ONE = getPhasePWM(    step);
 u1 const TWO = getPhasePWM(255-step);
 
 if(phase == Phase::A){
   cachedOCRB = TWO;
   cachedOCRC = ONE;
   stageNextInt(Phase::A);
 }
 else if(phase == Phase::B){
   cachedOCRA = ONE;
   cachedOCRC = TWO;
   stageNextInt(Phase::B);
 }
 else if(phase == Phase::C){
   cachedOCRA = TWO;
   cachedOCRB = ONE;
   stageNextInt(Phase::C);
 }
    
}

constexpr u1 INTdefault = 0b00000001;
void stageNextInt(const ThreePhaseDriver::Phase phase){
 if (phase = ThreePhaseDriver::Phase::A) cachedNextIntEnable = INTdefault & ~(1 << OCIE1A);
 else if (phase = ThreePhaseDriver::Phase::A) cachedNextIntEnable = INTdefault & ~(1 << OCIE1B);
 else if (phase = ThreePhaseDriver::Phase::A) cachedNextIntEnable = INTdefault & ~(1 << OCIE1C);
}

u1 ThreePhaseDriver::getPhasePWM(const u1 step) {
 u1 const sin = step;
 
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
