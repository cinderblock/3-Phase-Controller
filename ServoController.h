/**
 * The Servo controller for turnigy driver
 * 
 * Manages the servoing of hardware for 3-phase controller/driver
 * 
 * Takes in external inputs and outputs commands to ThreePhaseController
 */

#ifndef SERVOCONTROLLER_H
#define	SERVOCONTROLLER_H

#include <AVR++/basicTypes.h>
#include "DriverConstants.h"
#include "MLX90363.h"

namespace ThreePhaseControllerNamespace {

using namespace AVR;

/*
 * Static class for handling the manger
 */
class ServoController {
private:

  enum class Mode : u1 {
    Init, Amplitude, Velocity, Distance, Position, Phase
  };

  static Mode currentMode;

  static s2 amplitudeCommand;
  static s4 driveAmplitudeScaled;
  static s2 velocityCommand;
  static s4 positionCommand;
  static s4 distanceCommand;
  static s2 onRotation;
  static u2 zeroOffset;

  static u2 lastPosition;

  static s4 shiftingLimit;

  static u2 velocityAdjust;

  static u1 P;
  static u1 I;
  static u1 D;

  static u1 shift;

  static u1 currentLimit;

  static const u1 DeadBand = 50;

  static s4 command;

public:
	//initializer
  static void init();

  //main loop updater
  static void update();

 /*
  * control hardware
	*/

  //set an amplitude
  static void setAmplitude(s2);

  //set a velocity
  static void setVelocity(s2);
  //get the current velocity command
  inline static s2 getVelocityCommand() {
    return velocityCommand;
  };


  //angular position commands

  //set an angular position 
  static void setPosition(s4);
  //tell it to go a distance in rotational counts
  static void setDistance(s4);
	//get the current angular position
  inline static s4 getPosition(){return ((s4)onRotation * (1 << DriverConstants::MagnetometerBits)) + (DriverConstants::MagnetometerMax - MLX90363::getAlpha());};
  //get the current angular position commanded
	inline static s4 getPositionCommand() {return positionCommand;};

  //get current revolution count
  inline static s2 getRevolution(){return onRotation;};


  //set the current angular position as the zero
  static void setZero();
  

  //set to command a phase (stepper motor)
  inline static void setPhaseMode(){currentMode = Mode::Phase;};

  //set current limit
  static void setCurrentLimit(u1);

  //set the P value for the position command
  static inline void setP(u1 p) {P = p;};
  //get P for position command
  static inline u1 getP() {return P;};
  //set the I value for the position command (currently unused)
  // static inline void setI(u1 i) {I = i;};
  //set the D value for the position command (currently unused)
  static inline void setD(u1 d) {D = d;};
  //get D for position command
  static inline u1 getD() {return D;};
  //scaled up value for higher resolution in command 
  inline static s4 getShiftedCommand(){return command;};
  //set the shifted amount for scalling the position to be set
  static inline void setShift(u1 p) {shift = p;};
  //get the shift ammount
  static inline u1 getShift() {return shift;};

  //enable or disable servo controller
  static void setEnable(bool);

  //check if we are actully using the servo controller updates
  static inline bool isUpdating(){return currentMode != Mode::Init && currentMode != Mode::Phase;};

  //increment or decrement the rotation the servo is on
  inline static void incrementRotation(const bool up){onRotation += up ? 1 : -1;};

};

};

#endif	/* SERVOCONTROLLER_H */
