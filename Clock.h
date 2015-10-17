/* 
 * File:   Clock.h
 * Author: Cameron
 *
 * Created on October 16, 2015, 11:41 PM
 */

#ifndef CLOCK_H
#define	CLOCK_H

#include <avr/io.h>

#include "Board.h"

namespace Clock {
 inline void enablePLL(u1 const PLL = 10, u1 const timerDiv = 2, bool const useEXT = true) {
  PLLFRQ = ((useEXT ? 0 : 1) << PINMUX) | (timerDiv << PLLTM0) | (1 << PLLUSB) | PLL;
  bool const divideClock = useEXT && (Board::ClockSpeed == 16000000UL);
  PLLCSR = ((divideClock ? 1 : 0) << PINDIV) | (1 << PLLE);
 }
 
 inline void waitForPLL() {
  while (!(PLLCSR & (1 << PLOCK)));
 }
};

#endif	/* CLOCK_H */

