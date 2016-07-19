/**
 * The Servo controller for turnigy driver
 */

#ifndef SERVOCONTROLLER_H
#define	SERVOCONTROLLER_H

#include <AVR++/basicTypes.h>
#include "DriverConstants.h"
#include "MLX90363.h"

using namespace AVR;

class ServoController {
private:

  enum class Mode : u1 {
    Init, Amplitude, Velocity, Distance, Position
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

  static u1 Pshift;
  static u1 Dshift;

  static u1 currentLimit;

  static const u1 DeadBand = 50;

public:
  static void init();

  static void update();

  static void setAmplitude(s2);
  static void setVelocity(s2);

  inline static s2 getVelocityCommand() {
    return velocityCommand;
  };

  static void setZero();
  static void setPosition(s4);
  static void setDistance(s4);

  inline static s4 getPositionCommand() {return positionCommand;};
  inline static s4 getPosition(){return ((s4)onRotation * (1 << DriverConstants::MagnetometerBits)) + (DriverConstants::MagnetometerMax - MLX90363::getAlpha());};
  inline static s2 getRevolution(){return onRotation;};

  static void setCurrentLimit(u1);

  static inline void setP(u1 p) {P = p;};
  // static inline void setI(u1 i) {I = i;};
  static inline void setD(u1 d) {D = d;};

  static inline void setPshift(u1 p) {Pshift = p;};
  static inline void setDshift(u1 d) {Dshift = d;};

  static inline u1 getP() {return P;};
  static inline u1 getD() {return D;};

  static inline u1 getPshift() {return Pshift;};
  static inline u1 getDshift() {return Dshift;};

  static void setEnable(bool);

  inline static void incrementRotation(const bool up){onRotation += up ? 1 : -1;};

};

#endif	/* SERVOCONTROLLER_H */

