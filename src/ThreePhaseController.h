/*
 * File:   ThreePhaseController.h
 * Author: Cameron
 *
 * Created on October 22, 2015, 2:21 AM
 *
 * Takes in an amplitude to be commanded
 * handles input from predictor
 * outputs to the driver (hardware)
 */

#ifndef THREEPHASECONTROLLER_H
#define THREEPHASECONTROLLER_H

#include <AVR++/basicTypes.h>

#include "ThreePhaseDriver.h"
#include "ThreePhasePositionEstimator.h"
#include <avr/interrupt.h>

ISR(TIMER4_OVF_vect);

namespace ThreePhaseControllerNamespace {

using namespace Basic;

/**
 * This static class wraps around the ThreePhaseDriver and some position estimation
 * to continuously tell the Driver what phase to drive at for some constant amplitude drive.
 *
 * This presents the minimal interface that all "motors" should have:
 *  - Amplitude control of PWM
 *  - Position (and velocity) feedback estimations
 *
 * "Servos" for position, velocity, current, power, whatever are outside the scope
 * of this class. See ServoController for more.
 */
class ThreePhaseController {
  /**
   * Called periodically (nominally 31.25kHz) by TIMER 4 ISR
   */
  static inline void controlLoop() __attribute__((hot));
  friend void ::TIMER4_OVF_vect();

  volatile static bool enabled;

  /**
   * Are we trying to go forward
   */
  static volatile bool isForwardTorque;

  /**
   * 90 degree phase shift
   */
  static constexpr u1 output90DegPhaseShift = ThreePhaseDriver::StepsPerCycle / 4;

public:
  /**
   * Initialize the controller
   */
  static void init();

  /**
   * Enable the controller
   */
  inline static void enable() {
    // Enable Timer4 Overflow Interrupt so that controlLoop runs
    // Also disables any other Timer4 interrupts but they aren't being used.
    TIMSK4 = 1 << TOIE4;
  }

  /**
   * Disable the controller
   */
  inline static void disable() {
    // Disable Timer4 Overflow Interrupt
    // and all the other Timer4 interrupts because none of them are being used
    TIMSK4 = 0;
  }

  /**
   * Abstraction around possible amplitude values.
   *
   * Amplitude is an 8-bit number + a direction.
   *
   * We also store zero for speed. (Maybe this should not be done)
   */
  class Amplitude {
    bool forward;
    u1 amplitude;
    friend class ThreePhaseController;

  public:
    inline Amplitude(s2 const t) : forward(t >= 0), amplitude(forward ? t : -t){};

    inline Amplitude(const bool fwd, u1 const ampl) : forward(fwd), amplitude(ampl){};
  };

  /**
   * Set the desired drive amplitude
   */
  static void setAmplitude(const Amplitude t);

  /**
   * Get the current amplitude
   *
   * @return s2 (range [-255, 255])
   */
  static inline s2 getAmplitude() {
    return isForwardTorque ? ThreePhaseDriver::getAmplitude() : -(s2)(ThreePhaseDriver::getAmplitude());
  };
};

}; // namespace ThreePhaseControllerNamespace

#endif /* THREEPHASECONTROLLER_H */
