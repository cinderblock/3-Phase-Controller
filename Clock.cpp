/* 
 * File:   Clock.cpp
 * Author: Cameron
 * 
 * Created on October 20, 2015, 12:16 AM
 */

#include <avr/interrupt.h>

#include "Clock.h"
#include "Timer.h"

ISR(TIMER3_OVF_vect, __attribute__((alias("_ZN5Clock4tickEv"))));

u4 Clock::time;

void Clock::init() {
 Timer::init();
 time = 0;
 // Enable the overflow interrupt in the timer
 TIMSK3 = 1 << TOIE3;
}

void Clock::tick() {
 time++;
}

void Clock::readTime(u4& dest) {
 // Disable interrupts for the next few cycles
 cli();
 
 // Copy the time to the destination
 dest = time;
 
 // and re-enable the interrupts
 sei();
}



void Clock::readTime(MicroTime& dest) {
 // Disable interrupts for the next few cycles
 cli();
 
 // copy the times we care about
 u2 const micro = Timer::getCurTime();
 u4 t = time;
 
 const u1 pendingInts = TIMSK3;
 // and re-enable the interrupts
 sei();

 // If micro is small and there's a pending overflow, the real time has incremented
 if ((micro < 256) && (pendingInts & (1 << TOIE3)))
  t++;

 // Copy the times to the destination
 dest = micro;
 dest = t;
}