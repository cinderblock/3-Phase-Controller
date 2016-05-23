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
 

 static u1 magRoll;

 static const u1 MaxTorque = 40;
 
 /**
  * Number of cycles the PWM timer makes per measurement ready from MLX.
  * 
  * = frequency(PWM) * period(MLX) = 32kHz * 1.25ms = 40;
  */
 static constexpr u1 cyclesPWMPerMLX = 40;
 
 static bool isForwardTorque;

 /**
  * 90 degree phase shift
  */
 static constexpr u2 output90DegPhaseShift = ThreePhaseDriver::StepsPerCycle / 4;
 
public:
 static void init();

 class Torque {
  bool forward;
  u1 amplitude;
  friend class ThreePhaseController;

 public:
  inline Torque(s2 const t) : forward(t >= 0), amplitude(forward ? t : -t) {};
  inline Torque(const bool fwd, u1 const ampl) : forward(fwd), amplitude(ampl) {};
  // static Torque limitedTorque(s2 requestedTorque){return };
 };
 
 static void setTorque(const Torque t);
 static inline s2 getTorque(){return isForwardTorque ? ThreePhaseDriver::getAmplitude() : -(s2)(ThreePhaseDriver::getAmplitude());};

 static inline void setDeadTimes(u1 dt){ThreePhaseDriver::setDeadTimes(dt);};
 static inline u1 getDeadTimes(){return ThreePhaseDriver::getDeadTimes();};

 static bool updateDriver();
 
 inline static u4 getPredictedPosition(){return Predictor::getPredictedPosition();};
 inline static s2 getVelocity()         {return Predictor::getVelocity();};
 inline static u2 getMeasuredPosition() {return Predictor::getMeasuredPosition();}

 inline static constexpr u1 getMaxTorque(){return MaxTorque;};

};

#endif	/* THREEPHASECONTROLLER_H */

