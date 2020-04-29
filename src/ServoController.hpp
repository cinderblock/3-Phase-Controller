/**
 * The Servo controller for turnigy driver
 *
 * Manages the servoing of hardware for 3-phase controller/driver
 *
 * Takes in external inputs and outputs commands to ThreePhaseController
 */

#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include "Sensors/MLX90363.hpp"
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

    inline bool operator<(MultiTurn const &that) const { return (*this - that) < 0; }
  };

protected:
  /**
   * The number of turns we've moved and most recent sub-turn commutation
   */
  static MultiTurn position;

public:
  static inline MultiTurn getPosition() { return position; }

  typedef struct {
    u2 kP;
    u2 kI;
    u2 kD;
  } kPID;

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

  static kPID kPosition;
  static kPID kVelocity;

  static struct {
    struct {
      ThreePhaseController::Amplitude upper;
      ThreePhaseController::Amplitude lower;
    } push;
    struct {
      MultiTurn upper;
      MultiTurn lower;
    } position;
    struct {
      s2 upper;
      s2 lower;
    } velocity;
  } limits;

public:
  inline static void zeroTurns() { position.turns = 0; }

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
  inline static void setAmplitude(ThreePhaseController::Amplitude const amplitude) {
    servoMode = Mode::Amplitude;
    amplitudeCommand = amplitude;
  }

  /**
   * Puts the controller into constant velocity mode and sets the target
   * @param
   */
  inline static void setVelocity(s2 velocity) {
    servoMode = Mode::Velocity;
    velocityCommand = velocity;
  }

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
  inline static void setPosition(MultiTurn const &position) {
    servoMode = Mode::Position;
    positionCommand = position;
  }

  /**
   * Get the commanded position
   * @return
   */
  inline static MultiTurn getPositionCommand() { return positionCommand; }

  /**
   * Set Position PID
   * @param k the struct to copy
   */
  static inline void setPositionK(const kPID k) { kPosition = k; }

  /**
   * Set Velocity PID
   * @param k the struct to copy
   */
  static inline void setVelocityK(const kPID k) { kVelocity = k; }

  /**
   * Set P of position PID
   * @param p
   */
  static inline void setPosition_P(u2 p) { kPosition.kP = p; }

  /**
   * Get P of position PID
   * @return
   */
  static inline u2 getPosition_P() { return kPosition.kP; }

  /**
   * Set I of position PID
   * @param i
   */
  static inline void setPosition_I(u2 i) { kPosition.kI = i; }

  /**
   * Get I of position PID
   * @return
   */
  static inline u2 getPosition_I() { return kPosition.kI; }

  /**
   * Set D of position PID
   * @param d
   */
  static inline void setPosition_D(u2 d) { kPosition.kD = d; }

  /**
   * Get D of position PID
   * @return
   */
  static inline u2 getPosition_D() { return kPosition.kD; }

  /**
   * Set P of velocity PID
   * @param p
   */
  static inline void setVelocity_P(u2 p) { kVelocity.kP = p; }

  /**
   * Get P of velocity PID
   * @return
   */
  static inline u2 getVelocity_P() { return kVelocity.kP; }

  /**
   * Set I of velocity PID
   * @param i
   */
  static inline void setVelocity_I(u2 i) { kVelocity.kI = i; }

  /**
   * Get I of velocity PID
   * @return
   */
  static inline u2 getVelocity_I() { return kVelocity.kI; }

  /**
   * Set D of velocity PID
   * @param d
   */
  static inline void setVelocity_D(u2 d) { kVelocity.kD = d; }

  /**
   * Get D of velocity PID
   * @return
   */
  static inline u2 getVelocity_D() { return kVelocity.kD; }

  static inline void setAmplitudeLimit(const ThreePhaseController::Amplitude &a,
                                       const ThreePhaseController::Amplitude &b) {
    if (a < b) {
      limits.push.upper = b;
      limits.push.lower = a;
    } else {
      limits.push.upper = a;
      limits.push.lower = b;
    }
  }

  static inline void setVelocityLimit(const s2 a, const s2 b) {
    if (a < b) {
      limits.velocity.upper = b;
      limits.velocity.lower = a;
    } else {
      limits.velocity.upper = a;
      limits.velocity.lower = b;
    }
  }

  static inline void setPositionLimit(const MultiTurn a, const MultiTurn b) {
    if (a < b) {
      limits.position.upper = b;
      limits.position.lower = a;
    } else {
      limits.position.upper = a;
      limits.position.lower = b;
    }
  }

  /**
   * Disable servo controller
   */
  inline static void setEnable(bool enable) {
    if (enable)
      return;

    servoMode = Mode::Disabled;
  }
};

}; // namespace ThreePhaseControllerNamespace

#endif /* SERVOCONTROLLER_H */
