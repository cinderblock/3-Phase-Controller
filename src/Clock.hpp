/*
 * File:   Clock.h
 * Author: Cameron
 *
 * Created on October 20, 2015, 12:16 AM
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <AVR++/basicTypes.hpp>

#include <avr/interrupt.h>

#include "Timer.hpp"

namespace ThreePhaseControllerNamespace {

using namespace Basic;

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
    inline constexpr TickTime(u4 const ticks) : ticks(ticks){};
    inline constexpr TickTime() : ticks(){};
  };

  class MicroTime : public TickTime {
    u2 timerCount;

  public:
    inline constexpr MicroTime(u4 const ticks, u2 const timerCount = 0)
        : TickTime(ticks + timerCount / Timer::CountsPerClear), timerCount(timerCount % Timer::CountsPerClear){};
    inline constexpr MicroTime(unsigned long long const us)
        : TickTime(us / 1000), timerCount(((u2)(us % 1000)) * (Timer::CountsPerClear / 1000)){};
    inline constexpr MicroTime(long double const us)
        : TickTime(us / 1000), timerCount(((u2)((long long)us % 1000)) * (Timer::CountsPerClear / 1000)){};
    inline constexpr MicroTime() : TickTime(), timerCount(){};

    /**
     * Assign a new value to the sub millisecond part of this object
     * @param val
     * @return
     */
    inline MicroTime &setTimerCount(u2 const val) {
      timerCount = val;
      return *this;
    }
    inline MicroTime &setTicksCount(u4 const val) {
      ticks = val;
      return *this;
    }
    inline MicroTime &operator=(MicroTime const &that) {
      if (&that != this) {
        ticks = that.ticks;
        timerCount = that.timerCount;
      }
      return *this;
    }

    inline MicroTime &operator+=(MicroTime const &that) {
      timerCount += that.timerCount;
      ticks += that.ticks;
      if (timerCount >= Timer::CountsPerClear) {
        timerCount -= Timer::CountsPerClear;
        ticks++;
      }
      return *this;
    }

    inline MicroTime &operator-=(MicroTime const &that) {
      timerCount -= that.timerCount;
      ticks -= that.ticks;
      if (timerCount >= Timer::CountsPerClear) {
        timerCount += Timer::CountsPerClear;
        ticks--;
      }
      return *this;
    }

    inline MicroTime &operator+=(MicroTime &that) {
      timerCount += that.timerCount;
      ticks += that.ticks;
      if (timerCount >= Timer::CountsPerClear) {
        timerCount -= Timer::CountsPerClear;
        ticks++;
      }
      return *this;
    }

    inline MicroTime &operator-=(MicroTime &that) {
      timerCount -= that.timerCount;
      ticks -= that.ticks;
      if (timerCount >= Timer::CountsPerClear) {
        timerCount += Timer::CountsPerClear;
        ticks--;
      }
      return *this;
    }

    inline bool operator<(MicroTime const &that) const {
      if (ticks < that.ticks)
        return true;
      return (ticks == that.ticks) && (timerCount < that.timerCount);
    }

    inline bool operator<=(MicroTime const &that) const {
      if (ticks < that.ticks)
        return true;
      return (ticks == that.ticks) && (timerCount <= that.timerCount);
    }

    inline bool operator>(MicroTime const &that) const {
      if (ticks > that.ticks)
        return true;
      return (ticks == that.ticks) && (timerCount > that.timerCount);
    }

    inline bool operator>=(MicroTime const &that) const {
      if (ticks > that.ticks)
        return true;
      return (ticks == that.ticks) && (timerCount >= that.timerCount);
    }

    inline bool isInPast() const {
      MicroTime now;
      readTime(now);
      return *this < now;
    }

    inline bool isInFuture() const {
      MicroTime now;
      readTime(now);
      return *this > now;
    }

    inline u8 timerTicks() const { return ticks * Timer::CountsPerClear + timerCount; }
  };

  /**
   * Read the current time to a destination
   * @param dest
   */
  static void readTime(u4 &dest);

  static void readTime(TickTime &dest);

  static void readTime(MicroTime &dest);

  static void readTimeISR(MicroTime &dest);

  /**
   * If reading the time from an interrupt routine (really, anytime the global
   * interrupts are off) use this version of readTime()
   * @return the current time
   */
  inline static u4 readTimeISR() { return time; };
};

inline static constexpr const Clock::MicroTime operator"" _ms(unsigned long long const i) { return i * 1000; }

inline static constexpr const Clock::MicroTime operator"" _ms(long double const i) { return i * 1000; }

inline static constexpr const Clock::MicroTime operator"" _us(unsigned long long const i) { return i; }

}; // namespace ThreePhaseControllerNamespace

#endif /* CLOCK_H */
