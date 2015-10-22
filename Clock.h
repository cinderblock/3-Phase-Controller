/* 
 * File:   Clock.h
 * Author: Cameron
 *
 * Created on October 20, 2015, 12:16 AM
 */

#ifndef CLOCK_H
#define	CLOCK_H

#include <AVR++/basicTypes.h>

using namespace AVR;

class Clock {
 static u4 time;
 
public:
 
 static void tick();
 
 static void init();
 
 class TickTime {
 protected:
  u4 ticks;
 };
 
 class MicroTime : public TickTime {
  u2 timerCount;
 public:
  
  // This is a very hacky way to do this
  
  /**
   * Assign a new value to the sub millisecond part of this object
   * @param val
   * @return 
   */
  inline MicroTime& operator=(u2 const val) {timerCount = val; return *this;}
  inline MicroTime& operator=(u4 const val) {ticks = val; return *this;}
 };
 
 /**
  * Read the current time to a destination
  * @param dest
  */
 static void readTime(u4& dest);
 
 static void readTime(TickTime& dest);
 
 static void readTime(MicroTime& dest);
 
 /**
  * If reading the time from an interrupt routine (really, anytime the global
  * interrupts are off) use this version of readTime()
  * @return the current time
  */
 inline static u4 readTimeISR() {return time;};
 

};

#endif	/* CLOCK_H */

