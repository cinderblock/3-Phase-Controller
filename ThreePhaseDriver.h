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
#include "DriverConstants.h"

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
  static u1 amplitude;

public:

  /**
   * The outputs can be in one of 3 phases
   */
  enum class Phase : u1 {
    A = 0, B = 1, C = 2, Invalid
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
   
   friend class ThreePhaseDriver;

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

  public:
   /**
    * Initialize a commutation angle with some current angle
    * @param commutation
    */
   inline PhasePosition(u2 const commutation = 0) : commutation(commutation % (MAX + 1)) {}

   /**
    * Initialize a commutation angle with a subPhase angle and phase
    * @param phase
    * @param commutation
    */
   inline PhasePosition(Phase const phase, u1 const commutation) : commutation(((u1)phase << 8) | commutation) {}

   inline PhasePosition& operator+=(u1 const steps) {
    commutation += steps;
    if (commutation > MAX) commutation -= MAX;

    return *this;
   }

   inline PhasePosition& operator-=(u1 const steps) {
    commutation -= steps;
    if (commutation > MAX) commutation += MAX;

    return *this;
   }

   inline PhasePosition& operator+=(u2 const steps) {
    commutation += steps;
    commutation %= MAX + 1;

    return *this;
   }

   inline PhasePosition& operator-=(u2 const steps) {
    commutation -= steps;
    commutation %= MAX + 1;

    return *this;
   }

   inline PhasePosition& operator++() {
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

   inline operator u4() {
     return commutation;
   }
  };

  /**
   * Internal granularity of sin wave for each phase
   */
  static u2 constexpr StepsPerPhase = DriverConstants::StepsPerPhase;

  /**
   * One for each of A, B, and C.
   */
  static u1 constexpr PhasesPerCycle = DriverConstants::PhasesPerCycle;

  /**
   * One Cycle is one full commutation (aka electrical revolution) of the motor.
   * This is almost certainly not one actual revolution of the motor shaft.
   */
  static u2 constexpr StepsPerCycle = DriverConstants::StepsPerCycle;

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
  static void advanceTo(PhasePosition const phase);

  /**
   * Magic number to ensure we don't miss a tick of a phase
   */
  static constexpr u1 calcMaxAmplitude = 0xff - 30;
  static constexpr u1 maxAmplitude = min(DriverConstants::MaxTorque, calcMaxAmplitude);

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
    DT (u1 c) : combined(c) {}
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

