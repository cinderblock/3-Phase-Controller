/* 
 * File:   Timer.cpp
 * Author: Blake Tacklind
 *
 * Created on August 31, 2015, 5:55 PM
 */

#include "Timer.h"

void Timer::init() {
 /*
  * COM3A1  COM3A0  COM3B1  COM3B0  COM3C1  COM3C0    WGM31   WGM30
  * 0       0       0       0       0       0         0       0
  * Normal port   | Normal port     | Normal port    | CTC Mode
  * operations    | operations      | operations     |
  */
 TCCR3A = 0b00000000;

 /*
  * Setup for CTC mode but not yet running
  * ICNC3    ICES3   ~~~~~    WGM33    WGM32    CS32    CS31   CS30
  * 0        0                0        1        0       0      0
  * Noise   |Edge            | CTC OCR3A       | Clock
  * Canceler|Select          | Mode            | Initially stopped
  */
 // We'll using this const in a second
 u1 constexpr B = 0b00001000;
 TCCR3B = B;

 /*
  * Disable all interrupts
  * ~~~~~   ~~~~~   ICIE3   ~~~~~   OCIE3C  OCIE3B  OCIE0A  TOIE0 
  *                 0               0       0       0       0
  *                Input Capture   | Disabled Interrupts
  *               Interrupt Enable |
  */
 TIMSK3 = 0b00000000;

 // Start the timer at 0 just to be nice
 TCNT3 = 0;

 // Setup the correct timer TOP value
 OCR3A = TOP;

 // Clear all timer interrupt flags
 TIFR3 = 0xff;

 // Start the timer
 TCCR3B = B | CLKprescaler;

}

u2 Timer::getSince(const u2 time) {
 const u2 t = getCurTime();
 if (t > time)
  return t - time;
 
 return CountsPerClear + t - time;
}
