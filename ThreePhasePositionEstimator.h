
#ifndef THREEPHASEPOSITIONESTIMATOR_H
#define THREEPHASEPOSITIONESTIMATOR_H

#include <AVR++/basicTypes.h>
#include "DriverConstants.h"
#include "ThreePhaseDriver.h"
#include "MotorPosition.h"

namespace ThreePhaseControllerNamespace {

  using namespace AVR;

  /**
   * This class wraps around a rotor position estimate. At some regular interval,
   * we update our position estimate based on an estimate of our current velocity.
   * As some slower regular interval, we get real position readings. Those are used
   * to update our velocity estimates.
   *
   * This can be thought of as a software PLL. We're trying to interpolate positions
   * between actual readings from the magnetometer. If we're running to fast, slow
   * down our "period". If we're running too slow, speed up our "period". In this
   * case however, we have a fixed time based and we need to adjust how large our
   * steps are to match how fast we're really spinning.
   */
  class ThreePhasePositionEstimator {
    /**
     * Last magnetometer reading
     */
    static u2 lastMecPha;

    /**
     * The current position estimate. Higher resolution than output phase angle
     */
    static u4 drivePhase;

    /**
     * The current velocity estimate. Used to advance drivePhase position estimate
     */
    static s2 driveVelocity;

    /**
     * Amount to adjust the velocity by. The smaller this number is, the more
     * the velocity is filtered
     */
    static u1 adjustVal;

    /**
     * Multiplier on velocity to advance our velocity estimate by
     */
    static u1 phaseAdvanceRatio;

    /**
     * cached constant amount of phase advance for our current velocity
     */
    static s4 phaseAdvanceAmount;

    /**
     * For out internal estimate of position, increase the resolution by some number of bits
     */
    constexpr static u1 predictionResolutionShift = 8;

    /**
     * Number of MLX readings started since last estimate
     */
    static u1 mlxReadingsStarted;

    /**
     * Number of cycles the PWM timer makes per measurement ready from MLX. We pick
     * a number such that we wait at least 1ms between measurements, otherwise the
     * data won't be ready.
     *
     * = frequency(PWM) * period(MLX) = 32kHz * 1.25ms = 40;
     */
    static constexpr u1 cyclesPWMPerMLX = 40;

    /**
     * Converts from magnetometer lookup table numbers to linear numbers
     */
    inline static u2 getMechPhase(u2 phase) {
      return (phase & DriverConstants::MaskForPhase) + (phase >> 12) * DriverConstants::StepsPerCycle;
    };

    static void getAndProcessNewHallState();

  public:
    /**
     * Initialize hardware and internal variables to match initial hardware state
     */
    static void init();

    /**
     * Handle new position reading from MLX system
     *
     * @param alpha raw reading from MLX
     */
    static void handleNewPositionReading(u2 alpha);

    /**
     * Advance our prediction of where we currently are by one dt.
     *
     * @return current estimate of PhasePosition
     */
    static ThreePhaseDriver::PhasePosition advance() __attribute__((hot));

    /**
     * Get the phase advance ratio
     *
     * Estimation of delay from last magnetometer reading and setting current phase
     */
    inline static u1 getPhaseAdvanceRatio() {
      return phaseAdvanceRatio;
    };

    /**
     * Set phase advance ratio
     * @param val
     */
    inline static void setPhaseAdvanceRatio(u1 val) {
      phaseAdvanceRatio = val;
    }

    /**
     * Estimation of distance given there are delays in system (phaseAdvanceRatio * velocity)
     */
    inline static s4 getPhaseAdvanceAmount() {
      return phaseAdvanceAmount;
    };

    /**
     * Get the last measured position
     */
    inline static u2 getMeasuredPosition() {
      return lastMecPha;
    };

    /**
     * Get currently extrapolated velocity
     */
    inline static s2 getVelocity() {
      return driveVelocity;
    };

    /**
     * Get value the velocity may get shifted by per velocity update
     */
    inline static u1 getAdjustVal() {
      return adjustVal;
    }

    inline static void setAdjustVal(u1 val) {
      adjustVal = val;
    }
  };

};

#endif  /* THREEPHASEPOSITIONESTIMATOR_H */
