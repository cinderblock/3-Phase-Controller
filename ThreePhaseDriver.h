/* 
 * File:   ThreePhaseDriver.h
 * Author: Cameron
 *
 * Created on December 17, 2014, 3:20 PM
 */

#ifndef THREEPHASEDRIVER_H
#define	THREEPHASEDRIVER_H

#include "basicTypes.h"
#include <avr/interrupt.h>

class ThreePhaseDriver {
 
 /**
  * Read the sin table
  * @param phase
  * @return 
  */
 static inline u1 getPhasePWM(u1 const phase) __attribute__((const));
 
 static u1 amplitude;
 
// static u1 cacheA;
// static register u1 cacheB asm("r11");
// static register u1 cacheC asm("r12");
public:
 enum class Phase : u1 {A=0, B=1, C=2, ERR};

protected:
 static Phase currentPhase;

public:
 /**
  * Internal granularity of sin wave for each phase
  */
 static u2 constexpr StepsPerPhase = 256;
 
 /**
  * One for each of A, B and C.
  */
 static u1 constexpr PhasesPerCycle = 3;
 
 /**
  * One Cycle is one full commutation "revolution" of the motor. This is almost
  * certainly not one actual revolution of the motor shaft.
  */
 static u2 constexpr StepsPerCycle = StepsPerPhase * PhasesPerCycle;

 static void init();
 
 /**
  * Convenience function with its own step counter.
  */
 static void advance();
 
 inline static void advanceTo(u2 const step) {advanceTo((Phase)step, step);}
 static void advanceTo(Phase const phase, u1 const step);
};

#endif	/* THREEPHASEDRIVER_H */

