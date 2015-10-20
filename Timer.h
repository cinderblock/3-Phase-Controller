/* 
 * File:   Timer.h
 * Author: Blake
 *
 * Created on August 31, 2015, 12:35 PM
 */

#ifndef TIMER_H
#define	TIMER_H

#include <AVR++/basicTypes.h>
#include <avr/io.h>

using namespace AVR;

class Timer{
public:
  static void init();
  
  //gets ticks on timer converted from time in units
  static const u2 lengthMS(const u2 ms);
  static const u2 lengthUS(const u4 us);
  
  //return current timer position
  inline static u2 getCurTime(){return TCNT3;};
  //difference in timer position since given time within 1 cycle
  static u2 getSince(const u2 time);
private:
  //clock prescaler on timer
  static const u1 CLKprescale = 0b100;
  static const u1 cyclesPerUS = 16;
  //finds the prescale division from a switch table
  static const u2 prescaleDiv();
  
};


#endif	/* TIMER_H */
