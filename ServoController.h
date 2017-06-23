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
      Init, Amplitude, Velocity, Distance, Position, Phase
    };

    /**
     * Which mode are we currently in
     */
    static Mode currentMode;

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

    /**
     * Used to prevent drive amplitude from going above this number
     */
    static s4 shiftingLimit;

    static u1 P;
    static u1 I;
    static u1 D;

    static u1 shift;

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
     * Put servo into phase mode (treat motor like a stepper)
     */
    inline static void setPhaseMode() {
      currentMode = Mode::Phase;
    };


    /**
     * Set P of position PID
     * @param p
     */
    static inline void setP(u1 p) {
      P = p;
    };

    /**
     * Get P of position PID
     * @return
     */
    static inline u1 getP() {
      return P;
    };

    /**
     * Set I of position PID
     * @param i
     */
    static inline void setI(u1 i) {
      I = i;
    };

    /**
     * Get I of position PID
     * @return
     */
    static inline u1 getI() {
      return I;
    };

    /**
     * Set D of position PID
     * @param d
     */
    static inline void setD(u1 d) {
      D = d;
    };

    /**
     * Get D of position PID
     * @return
     */
    static inline u1 getD() {
      return D;
    };

    /**
     * Set the shifted amount for scaling the position to be set
     */
    static inline void setShift(u1 p) {
      shift = p;
    };
    
    /**
     * Get the shift amount
     */
    static inline u1 getShift() {
      return shift;
    };

    /**
     * Enable or disable servo controller
     */
    static void setEnable(bool);

  };

};

#endif /* SERVOCONTROLLER_H */
