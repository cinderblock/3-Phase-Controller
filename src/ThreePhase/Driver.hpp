/*
 * File:   ThreePhaseDriver.h
 * Author: Cameron
 *
 * Created on December 17, 2014, 3:20 PM
 */

#ifndef THREEPHASEDRIVER_H
#define THREEPHASEDRIVER_H

#include <AVR++/basicTypes.hpp>
#include <avr/interrupt.h>

#include "Config.h"

namespace ThreePhaseControllerNamespace {

using namespace Basic;

/**
 */
static inline u1 constexpr min(u1 const a, u1 const b) { return a > b ? b : a; };

/**
 * This static class handles controlling the low level TIMER4 registers.
 *
 * It has one main interface, advanceTo(PhasePosition), that calculates the correct
 * PWM values for the outputs and switches them all to their new values atomically.
 */
class ThreePhaseDriver {
  /**
   * Read the specialized sine lookup table
   * @param phase
   * @return
   */
  static inline u2 getPhasePWM(u1 const phase) __attribute__((const));

  /**
   * The current PWM amplitude
   */
  static volatile u1 amplitude;

  static volatile u1 amplitudeLimit;

  /**
   * Should we use ATmega32u4's PWM6 mode?
   *
   * This may seem like it is correct for us by name but it is not quite what we want.
   */
  static constexpr bool usingPWM6 = false;

public:
  static void emergencyDisable();

  static void emergencyOK();
  /**
   * Internal granularity of sin wave for each phase
   */
  static u2 constexpr StepsPerPhase = 256;

  /**
   * One for each of A, B, and C.
   */
  static u1 constexpr PhasesPerCycle = 3;

  /**
   * One Cycle is one full commutation (aka electrical revolution) of the motor.
   * This is almost certainly not one actual revolution of the motor shaft.
   */
  static u2 constexpr StepsPerCycle = StepsPerPhase * PhasesPerCycle;

  static u1 constexpr CyclesPerRevolution =
#if defined(BED_CONTROLLER) || defined(HOVER_DRIVE)
      15
#elif defined QUANTUM_DRIVE
      7
#else
      1
#endif
      ;

  static u2 constexpr StepsPerRevolution = StepsPerCycle * CyclesPerRevolution;

  /**
   * The outputs can be in one of 3 phases
   */
  enum class Phase : u1 { A = 0, B = 1, C = 2 };

  /**
   * class to store and work with all possible output "angles"
   */
  class PhasePosition {
  protected:
    /**
     * Internal byte + n bits that fully describe an output angle
     */
    u2 commutation;

    /**
     * The maximum valid value commutation can have
     */
    static constexpr decltype(commutation) MAX = StepsPerRevolution - 1;

    static constexpr decltype(commutation) FULL = StepsPerRevolution;

    static constexpr decltype(commutation) INVALID = -1;

    friend class ThreePhaseDriver;

  public:
    /**
     * Get the current subphase angle
     * @return
     */
    inline u1 getPosition() const { return (u1)commutation; }

    /**
     * Get the current Phase
     * @return A, B, or C
     */
    inline Phase getPhase() const { return (Phase)((commutation / StepsPerPhase) % PhasesPerCycle); }

    inline u2 getRaw() const { return commutation; }

    inline bool isValid() const { return commutation != INVALID; }

    constexpr inline PhasePosition() : commutation(0) {}

    constexpr inline PhasePosition(bool const invalid) : commutation(invalid ? INVALID : 0) {}

    constexpr inline PhasePosition(u2 const commutation) : commutation(commutation % FULL) {}
    constexpr inline PhasePosition(u4 const commutation) : commutation(commutation % FULL) {}
    constexpr inline PhasePosition(s8 const commutation) : commutation((commutation % FULL + FULL) % FULL) {}

    constexpr inline PhasePosition(PhasePosition const &copy) : commutation(copy.commutation) {}

    /**
     * Convenience constructor
     */
    constexpr inline PhasePosition(int const c) : commutation((u2)c % FULL) {}

    /**
     * Initialize a commutation angle with a subPhase angle and phase
     * @param phase
     * @param commutation
     */
    inline PhasePosition(Phase const phase, u1 const commutation) : commutation(((u1)phase << 8) | commutation) {}

    inline PhasePosition &operator+=(u1 const steps) {
      commutation += steps;
      if (commutation > MAX)
        commutation -= FULL;

      return *this;
    }

    inline PhasePosition &operator-=(u1 const steps) {
      commutation -= steps;
      if (commutation > MAX)
        commutation += FULL;

      return *this;
    }

    inline PhasePosition &operator+=(u2 const steps) {
      // TODO: This is broken if steps is very large and overflows commutation
      commutation += steps;
      commutation %= FULL;

      return *this;
    }

    inline PhasePosition &operator+=(s2 const steps) {
      // TODO: This is broken if steps is very large and overflows commutation
      commutation += steps;

      while (commutation > MAX) {
        if (steps > 0)
          commutation -= FULL;
        else
          commutation += FULL;
      }

      return *this;
    }

    inline PhasePosition &operator-=(u2 const steps) {
      commutation -= steps;
      // TODO: Better & faster math here
      while (commutation > MAX)
        commutation += FULL;

      return *this;
    }

    inline PhasePosition &operator++() {
      if (commutation == MAX) {
        commutation = 0;
      } else {
        commutation++;
      }

      return *this;
    }

    inline PhasePosition &operator--() {
      if (commutation == 0) {
        commutation = MAX;
      } else {
        commutation--;
      }

      return *this;
    }

    inline PhasePosition operator++(int) {
      PhasePosition ret(commutation);

      ++*this;

      return ret;
    }

    inline PhasePosition operator--(int) {
      PhasePosition ret(commutation);

      --*this;

      return ret;
    }

    inline s2 operator-(PhasePosition const &that) {
      s2 delta = commutation - that.commutation;

      const u2 half = FULL / 2;

      if (delta > s2(half))
        delta -= FULL;
      if (delta < s2(-half))
        delta += FULL;

      return delta;
    }

    inline s2 operator-(PhasePosition const &that) const {
      s2 delta = commutation - that.commutation;

      const u2 half = FULL / 2;

      if (delta > s2(half))
        delta -= FULL;
      if (delta < s2(-half))
        delta += FULL;

      return delta;
    }
  };

  /**
   * Initialize the AVR hardware
   */
  static void init();

  /**
   * Advance the pwm outputs to a new commutation angle
   *
   * @param phase
   * @param step
   */
  static void advanceTo(PhasePosition const phase) __attribute__((hot));

  static inline void setAmplitudeLimit(u1 const a) { amplitudeLimit = a; }

  static inline void setAmplitude(u1 const a) { amplitude = min(a, amplitudeLimit); }

  static inline u1 getAmplitude() { return amplitude; };

  using DeadTimes = union DT {
    /**
     * Two raw nibbles used by low level 3 phase outputs
     */
    u1 combined;
    struct {
      /**
       * Leading dead time
       */
      u1 leading : 4;

      /**
       * Trailing dead time
       */
      u1 trailing : 4;
    };
    inline DT(u1 c) : combined(c) {}
    inline DT(u1 lead, u1 trail) : leading(lead), trailing(trail) {}
    union DT operator+(u1 b) {
      return combined + b;
    }
    union DT operator-(u1 b) {
      return combined - b;
    }
  };

  static inline void setDeadTimes(DeadTimes dt) { DT4 = dt.combined; };

  static inline DeadTimes getDeadTimes() { return (DeadTimes)DT4; };
};

}; // namespace ThreePhaseControllerNamespace

#endif /* THREEPHASEDRIVER_H */
