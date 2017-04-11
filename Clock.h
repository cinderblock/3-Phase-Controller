/* 
 * File:   Clock.h
 * Author: Cameron
 *
 * Created on October 20, 2015, 12:16 AM
 */

#ifndef CLOCK_H
#define	CLOCK_H

#include <AVR++/basicTypes.h>

#include <avr/interrupt.h>

#include "Timer.h"

namespace ThreePhaseControllerNamespace {

using namespace AVR;

ISR(TIMER3_COMPA_vect);

class Clock {
 static u4 time;
 inline static void tick();
 friend void TIMER3_COMPA_vect();
 
public:
 
 
 static void init();
 
 class TickTime {
 protected:
  u4 ticks;
 public:
  inline TickTime(u4 const ticks) : ticks(ticks) {};
  inline TickTime() {};
 };
 
 class MicroTime : public TickTime {
  u2 timerCount;
 public:
  
  inline MicroTime(u4 const ticks, u2 const timerCount = 0) :
  TickTime(ticks + timerCount / Timer::CountsPerClear),
          timerCount(timerCount % Timer::CountsPerClear) {};
  inline MicroTime() : TickTime() {};
  
  /**
   * Assign a new value to the sub millisecond part of this object
   * @param val
   * @return 
   */
  inline MicroTime& setTimerCount(u2 const val) {timerCount = val; return *this;}
  inline MicroTime& setTicksCount(u4 const val) {ticks = val; return *this;}
  inline MicroTime& operator=(MicroTime const& that) {
   if (&that != this) {
    ticks = that.ticks;
    timerCount = that.timerCount;
   }
   return *this;
  }
  
  inline MicroTime& operator+=(MicroTime const& that) {
   timerCount += that.timerCount;
   ticks += that.ticks;
   if (timerCount >= Timer::CountsPerClear) {
    timerCount -= Timer::CountsPerClear;
    ticks++;
   }
   return *this;
  }
  
  inline bool operator<(MicroTime const& that) const {
   if (ticks < that.ticks)
    return true;
   return (ticks == that.ticks) && (timerCount < that.timerCount);
  }
  
  inline bool operator<=(MicroTime const& that) const {
   if (ticks < that.ticks)
    return true;
   return (ticks == that.ticks) && (timerCount <= that.timerCount);
  }
  
  inline bool operator>(MicroTime const& that) const {
   if (ticks > that.ticks)
    return true;
   return (ticks == that.ticks) && (timerCount > that.timerCount);
  }
  
  inline bool operator>=(MicroTime const& that) const {
   if (ticks > that.ticks)
    return true;
   return (ticks == that.ticks) && (timerCount >= that.timerCount);
  }
  
  inline bool isInPast() const {
   MicroTime now;
   readTime(now);
   return *this < now;
  }
 
 };
 
 /**
  * Read the current time to a destination
  * @param dest
  */
 static void readTime(u4& dest);
 
 static void readTime(TickTime& dest);
 
 static void readTime(MicroTime& dest);
 
 static void readTimeISR(MicroTime& dest);
 
 /**
  * If reading the time from an interrupt routine (really, anytime the global
  * interrupts are off) use this version of readTime()
  * @return the current time
  */
 inline static u4 readTimeISR() {return time;};
 

};

inline static Clock::MicroTime operator"" _ms(unsigned long long i) {
 return {(u4)i,0};
}

inline static Clock::MicroTime operator"" _us(unsigned long long i) {
 return {(u4)(i/1000),((u2)i % 1000) * (Timer::CountsPerClear / 1000)};
}

};

#endif	/* CLOCK_H */
