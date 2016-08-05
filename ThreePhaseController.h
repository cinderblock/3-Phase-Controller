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

  //are we trying to go forward
  static bool isForwardTorque;
  //are we trying to stop
  static bool isZeroTorque;

  /**
   * 90 degree phase shift
   */
  static constexpr u2 output90DegPhaseShift = ThreePhaseDriver::StepsPerCycle / 4;

public:
  //initilize the controller
  static void init();

  //update the driver
  static bool updateDriver();

  //class for holding amplitude commanded
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

  //uses an amplitude object to set the desired amplitude
  static void setAmplitude(const Amplitude t);

  //get the current amplitude as an s2 (range [-255, 255])
  static inline s2 getAmplitude() {return isForwardTorque ? ThreePhaseDriver::getAmplitude() : -(s2) (ThreePhaseDriver::getAmplitude());};

  //set the dead times between on and off on a single half h-bridge
  //contains 2 nibles
  static inline void setDeadTimes(u1 dt) {ThreePhaseDriver::setDeadTimes(dt);};
  //get the dead times between 
  static inline u1 getDeadTimes() {return ThreePhaseDriver::getDeadTimes();};

  //get the currently predicted angular position
  inline static u4 getPredictedPosition() {return Predictor::getPredictedPosition();};

  //get the currently predicted angular velocity
  inline static s2 getVelocity() {return Predictor::getVelocity();};

  //get the last measured position (by the magnetometer)
  inline static u2 getMeasuredPosition() {return Predictor::getMeasuredPosition();}

  //get the number of magnetometer measurments that have happened
  inline static u2 getRoll() {return roll;};

};

#endif	/* THREEPHASECONTROLLER_H */

