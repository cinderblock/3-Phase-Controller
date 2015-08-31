/* 
 * File:   Timer.h
 * Author: Cameron
 *
 * Created on August 31, 2015, 12:35 PM
 */

#ifndef TIMER_H
#define	TIMER_H


class Timer{
public:
  static void init();
  
  //gets ticks on timer converted from time in units
  static const u2 lengthMS(const u2 ms);
  static const u2 lengthUS(const u4 us);
  
  //return current timer position
  static u2 getCurTime() const;
  //difference in timer position since given time within 1 cycle
  static u2 getSince(const u2 time);
private:
  //clock prescaler on timer
  static const b3 CLKprescale = 0b100;
  //finds the prescale division from a switch table
  static const u2 prescaleDiv();
  
};


#endif	/* TIMER_H */
