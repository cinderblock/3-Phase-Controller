/**
 * The Servo controller for turnigy driver
 * 
 * Manages the servoing of hardware for 3-phase controller/driver
 * 
 * Takes in external inputs and outputs commands to ThreePhaseController
 */

#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <AVR++/basicTypes.h>
#include "DriverConstants.h"
#include "MLX90363.h"
#include "ThreePhaseDriver.h"
#include "ThreePhaseController.h"

namespace ThreePhaseControllerNamespace {

  using namespace AVR;

  /*
   * Static class for handling the various servo modes
   */
  class ServoController {
  private:
    /**
     * The different modes that we can use to servo
     */
    enum class Mode : u1 {
      Init, Amplitude, Velocity, Position
    };

    /**
     * Which mode are we currently in
     */
    static Mode servoMode;

    /**
     * Current command for simple amplitude control
     */
    static s2 amplitudeCommand;

    /**
     * Current drive amplitude in higher resolution than we can actually use
     */
    static s4 driveAmplitudeScaled;

    /**
     * The current target velocity
     */
    static s2 velocityCommand;

    /**
     * The current target position
     */
    static s4 positionCommand;

    /**
     * Number of full motor mechanical revolutions we've been through
     */
    static s2 onRotation;

    static u1 positionP;
    static u1 positionI;
    static u1 positionD;

    static u1 velocityP;
    static u1 velocityI;
    static u1 velocityD;

    static u1 positionShift;
    static u1 velocityShift;

    static const u1 DeadBand = 50;

  public:

    /**
     * Update amplitude command in ThreePhaseController based on current servo target parameters.
     */
    static void update();

    /**
     * Initialize all the hardware. Also run init() functions of lower level software.
     */
    static void init();

    /**
     * Set the controller into constant amplitude mode with some amplitude
     */
    static void setAmplitude(s2);

    /**
     * Puts the controller into constant velocity mode and sets the target
     * @param
     */
    static void setVelocity(s2);

    /**
     * Get the commanded velocity
     * @return
     */
    inline static s2 getVelocityCommand() {
      return velocityCommand;
    };


    //////////// angular position commands ////////////////

    /**
     * Put controller into position mode and sets the target
     * @param
     */
    static void setPosition(s4);

    /**
     * Put controller into position mode and sets the target as a delta from current
     * @param
     */
    static void setDistance(s4);

    /**
     * Get our current position
     * @return
     */
    inline static s4 getPosition() {
      return ((s4) onRotation * (1 << DriverConstants::MagnetometerBits)) + (DriverConstants::MagnetometerMax - MLX90363::getAlpha());
    };

    /**
     * Get the commanded position
     * @return
     */
    inline static s4 getPositionCommand() {
      return positionCommand;
    };

    /**
     * Get the current number of revolutions
     * @return 
     */
    inline static s2 getRevolution() {
      return onRotation;
    };


    /**
     * Set P of position PID
     * @param p
     */
    static inline void setPositionP(u1 p) {
      positionP = p;
    };

    /**
     * Get P of position PID
     * @return
     */
    static inline u1 getPositionP() {
      return positionP;
    };

    /**
     * Set I of position PID
     * @param i
     */
    static inline void setPositionI(u1 i) {
      positionI = i;
    };

    /**
     * Get I of position PID
     * @return
     */
    static inline u1 getPositionI() {
      return positionI;
    };

    /**
     * Set D of position PID
     * @param d
     */
    static inline void setPositionD(u1 d) {
      positionD = d;
    };

    /**
     * Get D of position PID
     * @return
     */
    static inline u1 getPositionD() {
      return positionD;
    };

    /**
     * Set the shifted amount for scaling the position PID values
     */
    static inline void setPositionShift(u1 p) {
      positionShift = p;
    };

    /**
     * Get the shift amount
     */
    static inline u1 getPositionShift() {
      return positionShift;
    };


    /**
     * Set P of velocity PID
     * @param p
     */
    static inline void setVelocityP(u1 p) {
      velocityP = p;
    };

    /**
     * Get P of velocity PID
     * @return
     */
    static inline u1 getVelocityP() {
      return velocityP;
    };

    /**
     * Set I of velocity PID
     * @param i
     */
    static inline void setVelocityI(u1 i) {
      velocityI = i;
    };

    /**
     * Get I of velocity PID
     * @return
     */
    static inline u1 getVelocityI() {
      return velocityI;
    };

    /**
     * Set D of velocity PID
     * @param d
     */
    static inline void setVelocityD(u1 d) {
      velocityD = d;
    };

    /**
     * Get D of velocity PID
     * @return
     */
    static inline u1 getVelocityD() {
      return velocityD;
    };

    /**
     * Set the shifted amount for scaling the velocity to be set
     */
    static inline void setVelocityShift(u1 p) {
      velocityShift = p;
    };

    /**
     * Get the velocity shift amount
     */
    static inline u1 getVelocityShift() {
      return velocityShift;
    };

    /**
     * Enable or disable servo controller
     */
    static void setEnable(bool);

  };

};

#endif /* SERVOCONTROLLER_H */
