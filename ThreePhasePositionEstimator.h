
#ifndef THREEPHASEPOSITIONESTIMATOR_H
#define THREEPHASEPOSITIONESTIMATOR_H

#include "ThreePhaseDriver.h"
#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {

using namespace AVR;

/**
 * This class wraps around a rotor position estimate. At some regular interval,
 * we update our position estimate based on an estimate of our current velocity.
 * As some slower regular interval, we get real position readings. Those are
 * used to update our velocity estimates.
 *
 * This can be thought of as a software PLL. We're trying to interpolate
 * positions between actual readings from the magnetometer. If we're running to
 * fast, slow down our "period". If we're running too slow, speed up our
 * "period". In this case however, we have a fixed time based and we need to
 * adjust how large our steps are to match how fast we're really spinning.
 */
class ThreePhasePositionEstimator {

  /**
   * The current position estimate. Higher resolution than output phase angle
   */
  static u4 volatile drivePhaseMagEstimate;

  /**
   * For out internal estimate of position, increase the resolution by some
   * number of bits
   */
  constexpr static u1 drivePhaseMagSubResolution = 8;

  /**
   * The current velocity estimate. Used to advance drivePhaseMagEstimate
   * position estimate
   */
  static s2 driveVelocityMagEstimate;

  /**
   * Multiplier on velocity error to adjust our velocity estimate
   */
  static u1 phaseAdvanceMagRatio;

  /**
   * cached constant amount of phase advance for our current velocity
   */
  static s4 phaseAdvanceMagCachedAmount;

  /**
   * Number of MLX readings started since last estimate
   */
  static u1 mlxPeriodsSinceLastValid;

  /**
   * Number of cycles the PWM timer makes per measurement ready from MLX. We pick a number such that we wait at least
   * 920us (tReady_mod1) between SS rising and falling edges, otherwise the data won't be ready.
   *
   * min(cyclesPWMPerMLX) = frequency(PWM) * period(MLX);
   *
   * period(MLX) = 920us + SPI transfer time
   *
   * SPI transfer time = 8 bytes * (10 / byte) / BAUD
   *
   * BAUD = F_CPU / SPI Divider
   *
   * SPI Divider = 128
   *
   * 10 bit time periods per byte to account for interrupt service length variations
   *
   * SPI transfer time = 640us
   *
   * period(MLX) = 1560us
   *
   * min(cyclesPWMPerMLX) = 31.25kHz * 1560us = 49;
   */
  static constexpr u1 cyclesPWMPerMLX = 49;

  /**
   * Number indicating the quality of our magnetometer based position estimate. Lower is worse.
   */
  static u1 qualityMagEstimate;

  /**
   * Most recent hall watcher state
   */
  static u1 lastHallState;


  /**
   *
   */
  static ThreePhaseDriver::PhasePosition drivePhaseHallEstimate;

  /**
   * Handle new position reading from Hall system
   *
   */
  static void getAndProcessNewHallState();

  /**
   * Handle new position reading from MLX system
   *
   * @param alpha raw reading from MLX
   */
  static void handleNewMagnetometerPositionReading(u2 alpha);

public:
  /**
   * Initialize hardware and internal variables to match initial hardware state
   */
  static void init();

  static constexpr u4 StepsPerRevolution = u4(ThreePhaseDriver::StepsPerRevolution) << drivePhaseMagSubResolution;
  
  /**
   * Advance our prediction of where we currently are by one dt.
   *
   * @return current estimate of PhasePosition
   */
  static ThreePhaseDriver::PhasePosition advance(u1 steps) __attribute__((hot));

  /**
   * Get the phase advance ratio
   *
   * Estimation of delay from last magnetometer reading and setting current
   * phase
   */
  inline static u1 getMagnetometerPhaseAdvanceRatio() { return phaseAdvanceMagRatio; };

  /**
   * Set phase advance ratio
   * @param val
   */
  inline static void setMagnetometorPhaseAdvanceRatio(u1 val) { phaseAdvanceMagRatio = val; }

  /**
   * Estimation of distance given there are delays in system (phaseAdvanceRatio
   * * velocity)
   */
  inline static s4 getCachedMagnetometerPhaseAdvanceAmount() { return phaseAdvanceMagCachedAmount; };

  /**
   * Get the last measured position
   */
  inline static typeof(drivePhaseMagEstimate) getMagnetometerPhaseEstimate() { return drivePhaseMagEstimate; };

  /**
   * Get currently extrapolated velocity
   */
  inline static s2 getMagnetometerVelocityEstimate() { return driveVelocityMagEstimate; };
};

}; // namespace ThreePhaseControllerNamespace

#endif /* THREEPHASEPOSITIONESTIMATOR_H */
