/* 
 * File:   Clock.h
 * Author: Cameron
 *
 * Created on October 20, 2015, 12:16 AM
 */

#ifndef CLOCK_H
#define	CLOCK_H

class Clock {
public:
 
 class TickTime {
  u4 ticks;
 };
 
 class MicroTime : public TickTime {
  u2 timerCount;
 };
 

};

#endif	/* CLOCK_H */

