/* 
 * File:   ThreePhaseController.h
 * Author: Cameron
 *
 * Created on October 22, 2015, 2:21 AM
 */

#ifndef THREEPHASECONTROLLER_H
#define	THREEPHASECONTROLLER_H

#include <AVR++/basicTypes.h>

#include "ThreePhaseDriver.h"
#include <avr/interrupt.h>
#include "Predictor.h"

using namespace AVR;

ISR(TIMER4_OVF_vect);

class ThreePhaseController {
  static inline void isr();
  friend void TIMER4_OVF_vect();

  static const u1 MaxAmplitude = 40;

  static u1 magRoll;
  static u2 roll;

  /**
   * Number of cycles the PWM timer makes per measurement ready from MLX. We pick
   * a number such that we wait at least 1ms between measurements, otherwise the
   * data won't be ready.
   *
   * = frequency(PWM) * period(MLX) = 32kHz * 1.25ms = 40;
   */
  static constexpr u1 cyclesPWMPerMLX = 40;

  static bool isForwardTorque;
  static bool isZeroTorque;

  /**
   * 90 degree phase shift
   */
  static constexpr u2 output90DegPhaseShift = ThreePhaseDriver::StepsPerCycle / 4;

public:
  static void init();

  class Amplitude {
    bool forward;
    bool zero;
    u1 amplitude;
    friend class ThreePhaseController;

  public:

    inline Amplitude(s2 const t) : forward(t >= 0), zero(t==0), amplitude(forward ? t : -t) {
    };

    inline Amplitude(const bool fwd, u1 const ampl) : forward(fwd), zero(ampl==0), amplitude(ampl) {
    };
    // static Torque limitedTorque(s2 requestedTorque){return };
  };

  static void setAmplitude(const Amplitude t);

  static inline s2 getAmplitude() {
    return isForwardTorque ? ThreePhaseDriver::getAmplitude() : -(s2) (ThreePhaseDriver::getAmplitude());
  };

  static inline void setDeadTimes(u1 dt) {
    ThreePhaseDriver::setDeadTimes(dt);
  };

  static inline u1 getDeadTimes() {
    return ThreePhaseDriver::getDeadTimes();
  };

  static bool updateDriver();

  inline static u4 getPredictedPosition() {
    return Predictor::getPredictedPosition();
  };

  inline static s2 getVelocity() {
    return Predictor::getVelocity();
  };

  inline static u2 getMeasuredPosition() {
    return Predictor::getMeasuredPosition();
  }

  inline static u2 getRoll() {
    return roll;
  };

  inline static constexpr u1 getMaxAmplitude() {
    return MaxAmplitude;
  };

};

#endif	/* THREEPHASECONTROLLER_H */

