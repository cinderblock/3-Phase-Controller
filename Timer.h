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

namespace ThreePhaseControllerNamespace {

using namespace Basic;

class Timer {
	/**
	 * clock prescaler on timer: /64
	 */
	static constexpr u1 CLKprescaler = 1;
	
  static constexpr u1 shiftMap[] = {0xff, 0, 3, 6, 8, 10};

  static constexpr u1 dividerShift = shiftMap[CLKprescaler];
	
	static constexpr u2 divider = 1 << dividerShift;
	
public:
	/**
	 * The number of counts that the timer will count to before overflowing
	 */
	static constexpr u2 CountsPerClear = 16000;
	
private:
	static constexpr u2 TOP = CountsPerClear - 1;
	
	static constexpr u4 Frequency = F_CPU / (divider * (1 + TOP));
	
public:
	
	static void init();

	/**
	 * Return current timer value
	 */
	inline static u2 getCurTime() {return TCNT3;};
	
	/**
	 * Difference between an old time and now
	 */
	static u2 getSince(const u2 time);

};

};


#endif	/* TIMER_H */
