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
  inline static void setPhaseMode(){currentMode = Mode::Phase;};

  inline static s4 getPositionCommand() {return positionCommand;};
  inline static s4 getPosition(){return ((s4)onRotation * (1 << DriverConstants::MagnetometerBits)) + (DriverConstants::MagnetometerMax - MLX90363::getAlpha());};
  inline static s2 getRevolution(){return onRotation;};
  inline static s4 getShiftedCommand(){return command;};

  static void setCurrentLimit(u1);

  static inline void setP(u1 p) {P = p;};
  // static inline void setI(u1 i) {I = i;};
  static inline void setD(u1 d) {D = d;};

  static inline void setShift(u1 p) {shift = p;};

  static inline u1 getP() {return P;};
  static inline u1 getD() {return D;};

  static inline u1 getShift() {return shift;};

  static void setEnable(bool);

  static inline bool isUpdating(){return currentMode != Mode::Init && currentMode != Mode::Phase;};

  inline static void incrementRotation(const bool up){onRotation += up ? 1 : -1;};

};

#endif	/* SERVOCONTROLLER_H */

