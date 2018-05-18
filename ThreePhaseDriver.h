/*
 * File:   ThreePhaseDriver.h
 * Author: Cameron
 *
 * Created on December 17, 2014, 3:20 PM
 */

#ifndef THREEPHASEDRIVER_H
#define	THREEPHASEDRIVER_H

#include <AVR++/basicTypes.h>
#include <avr/interrupt.h>

ISR(ADC_vect);

namespace ThreePhaseControllerNamespace {

using namespace AVR;

/**
 */
static inline u1 constexpr min(u1 const a, u1 const b){return a > b ? b : a;};

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

  static constexpr bool usingPWM6 = false;
   
  friend void ::ADC_vect();
  
  inline static void emergencyDisable();
  
  inline static void emergencyOK();

public:

  /**
   * The outputs can be in one of 3 phases
   */
  enum class Phase : u1 {
    A = 0, B = 1, C = 2, Brake = 3
  };

  /**
   * class to store and work with all possible output "angles"
   */
  class PhasePosition {
   /**
    * Internal byte + 2 bits that fully describe an output angle
    */
   u2 commutation;

   /**
    * The maximum valid value commutation can have
    */
   static constexpr u2 MAX = 0x2ff;

   static constexpr u2 FULL = MAX + 1;

   friend class ThreePhaseDriver;

  public:

   /**
    * Get the current subphase angle
    * @return
    */
   inline u1 getPosition() const {
    return (u1)commutation;
   }

   /**
    * Get the current Phase
    * @return
    */
   inline Phase getPhase() const {
    return (Phase)(commutation >> 8);
   }
   /**
    * Initialize a commutation angle with some current angle
    * @param commutation
    */
   inline PhasePosition(u2 const commutation = 0) : commutation(commutation % FULL) {}
   inline PhasePosition(u4 const commutation) : commutation(commutation % FULL) {}
   inline PhasePosition(int const commutation) : commutation(commutation % FULL) {}

   /**
    * Initialize a commutation angle with a subPhase angle and phase
    * @param phase
    * @param commutation
    */
   inline PhasePosition(Phase const phase, u1 const commutation) : commutation(((u1)phase << 8) | commutation) {}

   inline PhasePosition& operator+=(u1 const steps) {
     if (getPhase() == Phase::Brake) return *this;
    commutation += steps;
    if (commutation > MAX) commutation -= FULL;

    return *this;
   }

   inline PhasePosition& operator-=(u1 const steps) {
     if (getPhase() == Phase::Brake) return *this;
    commutation -= steps;
    if (commutation > MAX) commutation += FULL;

    return *this;
   }

   inline PhasePosition& operator+=(u2 const steps) {
     if (getPhase() == Phase::Brake) return *this;
     // TODO: This is broken if steps is very large and overflows commutation
    commutation += steps;
    commutation %= FULL;

    return *this;
   }

   inline PhasePosition& operator-=(u2 const steps) {
    if (getPhase() == Phase::Brake) return *this;
    commutation -= steps;
    // TODO: Better & faster math here
    while (commutation > MAX) commutation += FULL;

    return *this;
   }

   inline PhasePosition& operator++() {
    if (getPhase() == Phase::Brake) return *this;
    if (commutation == MAX) {
     commutation = 0;
    } else {
     commutation++;
    }

    return *this;
   }

   inline PhasePosition& operator--() {
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

   inline operator u4() const {
     return commutation;
   }

   inline s2 operator -(PhasePosition &that) {
     s2 delta = commutation - that.commutation;

     if (delta > s2(  FULL / 2)) delta -= MAX;
     if (delta < s2(- FULL / 2)) delta += MAX;

     return delta;
   }
  };

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

  /**
   * Highest possible timer value
   */
  static u2 constexpr MAX = 0x7ff;

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

  /**
   * Magic number to ensure we don't miss a tick of a phase
   * TODO: confirm this is still needed.
   */
  static constexpr u1 calcMaxAmplitude = 0xff - 30;
  static constexpr u1 maxAmplitude = calcMaxAmplitude;

  static inline void setAmplitude(u1 const a) {
    amplitude = a > maxAmplitude ? maxAmplitude : a;
  }

  static inline u1 getAmplitude() {
    return amplitude;
  };

  using DeadTimes = union DT {
    /**
     * Two raw nibbles used by low level 3 phase outputs
     */
    u1 combined;
    struct {
      /**
       * Leading dead time
       */
      u1 leading  :4;

      /**
       * Trailing dead time
       */
      u1 trailing :4;
    };
    inline DT (u1 c) : combined(c) {}
    inline DT (u1 lead, u1 trail) : leading(lead), trailing(trail) {}
    union DT operator+ (u1 b) {
      return combined + b;
    }
    union DT operator- (u1 b) {
      return combined - b;
    }
  };

  static inline void setDeadTimes(DeadTimes dt) {
    DT4 = dt.combined;
  };

  static inline DeadTimes getDeadTimes() {
    return (DeadTimes)DT4;
  };
};

};

#endif	/* THREEPHASEDRIVER_H */
