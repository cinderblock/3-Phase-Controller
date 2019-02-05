/* 
 * File:   AVRClock.h
 * Author: Cameron
 *
 * Created on October 16, 2015, 11:41 PM
 */

#ifndef AVRCLOCK_H
#define	AVRCLOCK_H

#include <avr/io.h>

#include "Board.hpp"

namespace AVR {
 namespace Clock {
   enum class PLLOutFreq : u1 {
     f40MHz = 0b0011,
     f48MHz = 0b0100,
     f56MHz = 0b0101,
     f72MHz = 0b0111,
     f80MHz = 0b1000,
     f88MHz = 0b1001,
     f96MHz = 0b1010,
   };
   
   enum class Timer4Div : u1 {
     Disabled = 0b00,
     One = 0b01,
     OneAndHalf = 0b10,
     Two = 0b11,
   };
   
   static constexpr Timer4Div minTimerDivForPLL(PLLOutFreq const out) {
     return (out >= PLLOutFreq::f72MHz ? Timer4Div::OneAndHalf : Timer4Div::One);
   }
   
   static constexpr auto defaultPLLOutFreq = PLLOutFreq::f96MHz;
   static constexpr bool defaultUseExternalCystal = true;
   
  inline void enablePLL(
    bool const useEXT,
    Timer4Div const timerDiv,
    PLLOutFreq const PLL
  ) {
   PLLFRQ = ((useEXT ? 0 : 1) << PINMUX) | (u1(timerDiv) << PLLTM0) | ((PLL == PLLOutFreq::f96MHz ? 1 : 0) << PLLUSB) | u1(PLL);
   
   bool const dividePLLInputClock = useEXT && (F_CPU == 16000000UL);
   
   PLLCSR = ((dividePLLInputClock ? 1 : 0) << PINDIV) | (1 << PLLE);
  }
  
  inline void enablePLL(
    bool const useEXT,
    PLLOutFreq const PLL = defaultPLLOutFreq
  ) {
    enablePLL(useEXT, minTimerDivForPLL(PLL), PLL);
  }
  
  inline void enablePLL(PLLOutFreq const PLL = defaultPLLOutFreq) {
    enablePLL(defaultUseExternalCystal, minTimerDivForPLL(PLL), PLL);
  }
  
  inline void enablePLL(Timer4Div const timerDiv, PLLOutFreq const PLL) {
    enablePLL(defaultUseExternalCystal, timerDiv, PLL);
  }

  inline void waitForPLL() {
   while (!(PLLCSR & (1 << PLOCK)));
  }
 };
};

#endif	/* AVRCLOCK_H */
