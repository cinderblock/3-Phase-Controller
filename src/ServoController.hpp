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
public:
  /**
   * The different modes that we can use to servo
   */
  enum class Mode : u1 { Disabled = 0, Amplitude = 1, Velocity = 2, Position = 3 };

private:
  /**
   * Which mode are we currently in
   */
  static Mode servoMode;

  using PhasePosition = ThreePhaseDriver::PhasePosition;

public:
  class MultiTurn : private PhasePosition {
  protected:
    /**
     * Number of turns
     */
    s4 turns;

    friend class ServoController;

  public:
    /**
     * Initialize a commutation angle with some current angle
     * @param value
     */
    constexpr inline MultiTurn(const s8 value = 0) : PhasePosition(value), turns(value / FULL) {}

    constexpr inline MultiTurn(const s4 turns, const PhasePosition pp) : PhasePosition(pp), turns(turns) {}

  private:
    /**
     * Update this instance to match a new phase position
     * @return the number of counts moved
     */
    inline s2 update(PhasePosition const &update) {
      const auto delta = update - *this;

      *this += delta;

      return delta;
    }

  public:
    inline MultiTurn &operator+=(u1 const steps) {
      commutation += steps;
      if (commutation > MAX) {
        commutation -= FULL;
        turns++;
      }

      return *this;
    }

    inline MultiTurn &operator-=(u1 const steps) {
      commutation -= steps;
      if (commutation > MAX) {
        commutation += FULL;
        turns--;
      }

      return *this;
    }

    inline MultiTurn &operator+=(u2 const steps) {
      // TODO: This is broken if steps is very large and overflows commutation
      commutation += steps;
      if (commutation > MAX) {
        commutation %= FULL;
        turns++;
      }

      return *this;
    }

    inline MultiTurn &operator+=(s2 const steps) {
      // TODO: This is broken if steps is very large and overflows commutation
      commutation += steps;

      while (commutation > MAX) {
        if (steps > 0) {
          commutation -= FULL;
          turns++;
        } else {
          commutation += FULL;
          turns--;
        }
      }

      return *this;
    }

    inline MultiTurn &operator-=(u2 const steps) {
      commutation -= steps;
      // TODO: Better & faster math here
      while (commutation > MAX) {
        commutation += FULL;
        turns--;
      }

      return *this;
    }

    inline MultiTurn &operator++() {
      if (commutation == MAX) {
        commutation = 0;
        turns++;
      } else {
        commutation++;
      }

      return *this;
    }

    inline MultiTurn &operator--() {
      if (commutation == 0) {
        commutation = MAX;
        turns--;
      } else {
        commutation--;
      }

      return *this;
    }

    inline MultiTurn operator++(int) {
      MultiTurn ret(commutation);

      ++*this;

      return ret;
    }

    inline MultiTurn operator--(int) {
      MultiTurn ret(commutation);

      --*this;

      return ret;
    }

    inline s8 operator-(MultiTurn const &that) {
      return s8(turns - that.turns) * FULL + commutation - that.commutation;
    }

    inline s8 operator-(MultiTurn const &that) const {
      return s8(turns - that.turns) * FULL + commutation - that.commutation;
    }
  };

protected:
  /**
   * The number of turns we've moved and most recent sub-turn commutation
   */
  static MultiTurn position;

public:
  static inline MultiTurn getPosition() { return position; }

protected:
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
  static MultiTurn positionCommand;

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
  static void zeroTurns();

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
  static void setAmplitude(ThreePhaseController::Amplitude const &);

  /**
   * Puts the controller into constant velocity mode and sets the target
   * @param
   */
  static void setVelocity(s2);

  /**
   * Get the commanded velocity
   * @return
   */
  inline static s2 getVelocityCommand() { return velocityCommand; }

  //////////// angular position commands ////////////////

  /**
   * Put controller into position mode and sets the target
   * @param
   */
  static void setPosition(MultiTurn const &);

  /**
   * Get the commanded position
   * @return
   */
  inline static MultiTurn getPositionCommand() { return positionCommand; }

  /**
   * Set P of position PID
   * @param p
   */
  static inline void setPosition_P(u2 p) { position_P = p; }

  /**
   * Get P of position PID
   * @return
   */
  static inline u2 getPosition_P() { return position_P; }

  /**
   * Set I of position PID
   * @param i
   */
  static inline void setPosition_I(u2 i) { position_I = i; }

  /**
   * Get I of position PID
   * @return
   */
  static inline u2 getPosition_I() { return position_I; }

  /**
   * Set D of position PID
   * @param d
   */
  static inline void setPosition_D(u2 d) { position_D = d; }

  /**
   * Get D of position PID
   * @return
   */
  static inline u2 getPosition_D() { return position_D; }

  /**
   * Set the shifted amount for scaling the position PID values
   */
  static inline void setPositionShift(u1 p) { positionShift = p; }

  /**
   * Get the shift amount
   */
  static inline u1 getPositionShift() { return positionShift; }

  /**
   * Set P of velocity PID
   * @param p
   */
  static inline void setVelocity_P(u2 p) { velocity_P = p; }

  /**
   * Get P of velocity PID
   * @return
   */
  static inline u2 getVelocity_P() { return velocity_P; }

  /**
   * Set I of velocity PID
   * @param i
   */
  static inline void setVelocity_I(u2 i) { velocity_I = i; }

  /**
   * Get I of velocity PID
   * @return
   */
  static inline u2 getVelocity_I() { return velocity_I; }

  /**
   * Set D of velocity PID
   * @param d
   */
  static inline void setVelocity_D(u2 d) { velocity_D = d; }

  /**
   * Get D of velocity PID
   * @return
   */
  static inline u2 getVelocity_D() { return velocity_D; }

  /**
   * Set the shifted amount for scaling the velocity to be set
   */
  static inline void setVelocityShift(u1 p) { velocityShift = p; }

  /**
   * Get the velocity shift amount
   */
  static inline u1 getVelocityShift() { return velocityShift; }

  /**
   * Enable or disable servo controller
   */
  static void setEnable(bool);
};

}; // namespace ThreePhaseControllerNamespace

#endif /* SERVOCONTROLLER_H */
