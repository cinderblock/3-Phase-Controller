/* 
 * File:   TimerTimeout.h
 * Author: Cameron
 *
 * Created on April 1, 2016, 3:47 PM
 */

#ifndef TIMERTIMEOUT_H
#define	TIMERTIMEOUT_H

#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {

using namespace AVR;

class TimerTimeout {
	static constexpr u2 divider = 1024;
	static constexpr u1 timerClockSelect = 0b101;

	static constexpr u1 MAX = 0xff;
	
	inline static void timeoutReset();
	
	inline static void timeoutEnable();
	inline static void timeoutDisable();
	
public:
	static void init();
	static void startA(u1 period);
	static void startB(u1 period);
	
	static void startAISR(u1 period);
	static void startBISR(u1 period);
	
	static void stopA();
	static void stopB();
	
	static void stopAISR();
	static void stopBISR();
	
	inline static constexpr u1 secondsToCount(long double s) {
		return s < 0 ? 0 : s > MAX * divider / (long double)F_CPU ? MAX : F_CPU / divider * s;
	}
	
	class Period {
		u1 counts;
	public:
		inline constexpr Period(long double s) : counts(secondsToCount(s)) {}
		inline constexpr Period(u1 counts) : counts(counts) {}
		inline constexpr Period(Period& p) : counts(p.counts) {}
		inline constexpr operator u1() const {
			return counts;
		}

	};
	
private:

};
		
	
inline constexpr TimerTimeout::Period operator "" _ms(long double ms) {
	return TimerTimeout::Period(ms / 1000);
}

inline constexpr TimerTimeout::Period operator "" _s(long double s) {
	return TimerTimeout::Period(s);
}

};

#endif	/* TIMERTIMEOUT_H */
