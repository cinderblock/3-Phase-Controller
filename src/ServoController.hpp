/**
 * The Servo controller for turnigy driver
 *
 * Manages the servoing of hardware for 3-phase controller/driver
 *
 * Takes in external inputs and outputs commands to ThreePhaseController
 */

#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include "MLX90363.hpp"
#include "ThreePhase/Controller.hpp"
#include <AVR++/basicTypes.hpp>

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
  enum class Mode : u1 { Init, Amplitude, Velocity, Position };

  /**
   * Which mode are we currently in
   */
  static Mode servoMode;

  /**
   * Current command for simple amplitude control
   */
  static ThreePhaseController::Amplitude amplitudeCommand;

  /**
   * The current target velocity
   */
  static s2 velocityCommand;

  /**
   * The current target position
   */
  static ThreePhaseDriver::PhasePosition positionCommand;

  /**
   * Number of full motor mechanical revolutions we've been through
   */
  static s2 onRotation;

  static u2 position_P;
  static u2 position_I;
  static u2 position_D;

  static u2 velocity_P;
  static u2 velocity_I;
  static u2 velocity_D;

  static u1 positionShift;
  static u1 velocityShift;

  // static u2 initialPhasePosition;

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
  inline static s2 getVelocityCommand() { return velocityCommand; };

  //////////// angular position commands ////////////////

  /**
   * Put controller into position mode and sets the target
   * @param
   */
  static void setPosition(ThreePhaseDriver::PhasePosition);

  /**
   * Put controller into position mode and sets the target as a delta from current
   *
   * @warning Broken for extreme values!
   *
   * @param
   */
  static void setDistance(s2);

  /**
   * Get the commanded position
   * @return
   */
  inline static s4 getPositionCommand() { return positionCommand; };

  /**
   * Get the current number of revolutions
   * @return
   */
  inline static s2 getRevolution() { return onRotation; };

  /**
   * Set P of position PID
   * @param p
   */
  static inline void setPosition_P(u2 p) { position_P = p; };

  /**
   * Get P of position PID
   * @return
   */
  static inline u2 getPosition_P() { return position_P; };

  /**
   * Set I of position PID
   * @param i
   */
  static inline void setPosition_I(u2 i) { position_I = i; };

  /**
   * Get I of position PID
   * @return
   */
  static inline u2 getPosition_I() { return position_I; };

  /**
   * Set D of position PID
   * @param d
   */
  static inline void setPosition_D(u2 d) { position_D = d; };

  /**
   * Get D of position PID
   * @return
   */
  static inline u2 getPosition_D() { return position_D; };

  /**
   * Set the shifted amount for scaling the position PID values
   */
  static inline void setPositionShift(u1 p) { positionShift = p; };

  /**
   * Get the shift amount
   */
  static inline u1 getPositionShift() { return positionShift; };

  /**
   * Set P of velocity PID
   * @param p
   */
  static inline void setVelocity_P(u2 p) { velocity_P = p; };

  /**
   * Get P of velocity PID
   * @return
   */
  static inline u2 getVelocity_P() { return velocity_P; };

  /**
   * Set I of velocity PID
   * @param i
   */
  static inline void setVelocity_I(u2 i) { velocity_I = i; };

  /**
   * Get I of velocity PID
   * @return
   */
  static inline u2 getVelocity_I() { return velocity_I; };

  /**
   * Set D of velocity PID
   * @param d
   */
  static inline void setVelocity_D(u2 d) { velocity_D = d; };

  /**
   * Get D of velocity PID
   * @return
   */
  static inline u2 getVelocity_D() { return velocity_D; };

  /**
   * Set the shifted amount for scaling the velocity to be set
   */
  static inline void setVelocityShift(u1 p) { velocityShift = p; };

  /**
   * Get the velocity shift amount
   */
  static inline u1 getVelocityShift() { return velocityShift; };

  /**
   * Enable or disable servo controller
   */
  static void setEnable(bool);
};

}; // namespace ThreePhaseControllerNamespace

#endif /* SERVOCONTROLLER_H */
