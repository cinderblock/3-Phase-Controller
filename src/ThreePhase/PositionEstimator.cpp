
#include "PositionEstimator.hpp"
#include "LookupTable.hpp"
// #include "Debug.hpp"
#include "Config.h"
#include "Controller.hpp"
#include "Debug.hpp"
#include "HallWatcher.hpp"
#include "MLX90363.hpp"
#include "ServoController.hpp"
#include <util/atomic.h>
#include <util/delay.h>

// #include "Board.hpp"
// #include <iostream>
// #include <cmath>
// #include "common.hpp"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

decltype(ThreePhasePositionEstimator::drivePhaseHallEstimate) ThreePhasePositionEstimator::drivePhaseHallEstimate;

decltype(ThreePhasePositionEstimator::drivePhaseMagEstimate) ThreePhasePositionEstimator::drivePhaseMagEstimate;

// Assume we start at 0 velocity
decltype(ThreePhasePositionEstimator::driveVelocityMagEstimate) ThreePhasePositionEstimator::driveVelocityMagEstimate =
    0;

decltype(ThreePhasePositionEstimator::phaseAdvanceMagRatio) ThreePhasePositionEstimator::phaseAdvanceMagRatio = 100;
decltype(
    ThreePhasePositionEstimator::phaseAdvanceMagCachedAmount) ThreePhasePositionEstimator::phaseAdvanceMagCachedAmount =
    0;
decltype(ThreePhasePositionEstimator::mlxPeriodsSinceLastValid) ThreePhasePositionEstimator::mlxPeriodsSinceLastValid =
    0;
decltype(ThreePhasePositionEstimator::qualityMagEstimate) ThreePhasePositionEstimator::qualityMagEstimate = 0;

inline static s2 constexpr abs(s2 num) { return num >= 0 ? num : -num; }

inline static void limit(u4 &value, u4 MAX, bool forward) __attribute__((hot));
inline static void limit(u4 &value, u4 MAX, bool forward) {
  while (value >= MAX) {
    if (forward) {
      value -= MAX;
    } else {
      // Looped negative. Fix it by looping back around.
      value += MAX;
    }
  }
}

/**
 * Class to help trigger a periodic task based on potentially arbitrary steps in time
 */
template <u1 period> class Counter {
  u1 count;

public:
  Counter() : count(period) {}

  u1 advanceAndCheckOverflow(u1 i) {

    u1 ret = 0;

    while (i) {
      if (count > i) {
        count -= i;
        break;
      }

      ret++;

      i -= count;

      count = period;
    };

    return ret;
  }
};

ThreePhaseDriver::PhasePosition ThreePhasePositionEstimator::advance(u1 steps) {
  // If cyclesPWMPerMLX is 1, will try to communicate over SPI every time advance() is called.
  static Counter<cyclesPWMPerMLX> mlxPeriodCounter;

  // Automatically start MLX communications every few ticks

  if (auto overflows = mlxPeriodCounter.advanceAndCheckOverflow(steps)) {
    // TODO: There is a potential issue that if the timings are too tight, in
    // certain cases, we could talk to the MLX late, which isn't immediately a
    // problem, except that the next MLX reading would be early, causing a NTT
    // response from the MLX. This isn't a major problem as the MLX handler
    // should just drop that sample and get the next one.

    MLX90363::startTransmitting();
    // overflows should almost always be 1, but just in case...
    mlxPeriodsSinceLastValid += overflows;
  }

  //  HallWatcher::checkH1();
  //  HallWatcher::checkH2();
  //  HallWatcher::checkH3();
  //
  //  return drivePhaseHallEstimate;

  u4 newPhaseEstimate = drivePhaseMagEstimate;
  newPhaseEstimate += driveVelocityMagEstimate * steps;

  const bool forward = driveVelocityMagEstimate > 0;

  // Check if ph(ase) value is out of range
  limit(newPhaseEstimate, StepsPerRevolution, forward);

  // Store new drivePhase
  drivePhaseMagEstimate = newPhaseEstimate;

  // Adjust output for velocity lag
  newPhaseEstimate += phaseAdvanceMagCachedAmount;

  // Check if ph(ase) value is out of range again
  limit(newPhaseEstimate, StepsPerRevolution, forward);

  // If we're going fast, use Hall position readings directly
  //  if (qualityMagEstimate < 100) {
  //    return drivePhaseHallEstimate;
  //  }

  return (newPhaseEstimate >> drivePhaseMagSubResolution);
}

void ThreePhasePositionEstimator::getAndProcessNewHallState() {
  u1 const state = HallWatcher::getState();

  if (state == 1)
    drivePhaseHallEstimate = 256;
  if (state == 2)
    drivePhaseHallEstimate = 0;
  if (state == 3)
    drivePhaseHallEstimate = 128;
  if (state == 4)
    drivePhaseHallEstimate = 512;
  if (state == 5)
    drivePhaseHallEstimate = 384;
  if (state == 6)
    drivePhaseHallEstimate = 640;
}

void ThreePhasePositionEstimator::handleNewMagnetometerPositionReading(u2 alpha) {
  // Nothing to do if lookup table is invalid
  if (!Lookup::isValid)
    return;

  // Here, we are receiving a new position reading from the magnetometer.
  // We need to take this new reading, update our running estimates, and be
  // done. This is called by the magnetometer software after a new reading has
  // been received and validated with CRC. We will need to handle issues like
  // missing a count.

  // While this is technically called from inside an ISR, interrupts have been
  // re-enabled. Therefore we need to consider the interplay between the 3 phase
  // ISR and this position estimator.

  // Disable this long handler. Re enabled at the end.
  ATOMIC_BLOCK(ATOMIC_FORCEON) { MLX90363::setAlphaHandler(nullptr); }

  // static u1 tick = 0;

  // Debug::SOUT
  //         << Debug::Printer::Special::Start
  //         << tick++
  //         << reading
  //         << Debug::Printer::Special::End;

  // Capture the number of times this should have been run.
  // If we don't compensate for this, our velocity estimates might be way wrong
  u1 const numberOfCycles = mlxPeriodsSinceLastValid;
  mlxPeriodsSinceLastValid = 0;

  // Instead of directly calculating what the velocity should be, for instance doing a dX/dt division,
  // we just keep a running estimate of the velocity and adjust it up or down based on how close we were.
  // This has the effect of doing a low pass filter on the velocity estimate which is desirable.

  auto v = driveVelocityMagEstimate;
  u4 estimate;

  // Capture our current position estimate
  ATOMIC_BLOCK(ATOMIC_FORCEON) { estimate = drivePhaseMagEstimate; }

  const u4 phase = u4(Lookup::AlphaToPhase(alpha));
  static u4 lastPhase = 0xFFFFFFFF;
  if (lastPhase == 0xFFFFFFFF)
    lastPhase = phase; // handle first case

  const u4 position = phase << drivePhaseMagSubResolution;

  // check for glitch
  // if (abs(phase - lastPhase) > 100  ) {
  //   MLX90363::setAlphaHandler(&handleNewMagnetometerPositionReading);
  //   Board::LED::on();
  //   return;
  // }

  // Positive delta likely means our velocity estimate is too fast
  s4 delta = position - estimate;

  // Fix delta range
  if (delta > s4(StepsPerRevolution / 2))
    delta -= StepsPerRevolution;
  if (-delta > s4(StepsPerRevolution / 2))
    delta += StepsPerRevolution;

  //  constexpr u1 MAXerr = 10;
  //  if (deltaError >  MAXerr) deltaError =  MAXerr;
  //  if (deltaError < -MAXerr) deltaError = -MAXerr;

  using namespace Debug;

  // Scale the error by some factor and adjust our velocity estimate
  v += delta / (cyclesPWMPerMLX * numberOfCycles * 8);

  ThreePhaseController::handleNewVelocityEstimate(v);

  //  SOUT << Printer::Special::Start
  //      << numberOfCycles << estimate << position << delta << v << MLX90363::getRoll()
  //      << Printer::Special::End;

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    driveVelocityMagEstimate = v;
    drivePhaseMagEstimate = position;
    // Re-enable this long alpha handler
    MLX90363::setAlphaHandler(&handleNewMagnetometerPositionReading);
  }
}

static u1 roll;

void ThreePhasePositionEstimator::init() {
  MLX90363::init();

  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  roll = MLX90363::getRoll();

  MLX90363::startTransmitting();

  // Set up the hall sensor interrupts
  // HallWatcher::init();

  //  Debug::dout << PSTR("Start: \n") << drivePhaseMagEstimate << '\n';
  //
  //  Debug::SOUT << Debug::Printer::Special::End;

  // HallWatcher::setStateChangeReceiver(&getAndProcessNewHallState);

  MLX90363::setAlphaHandler(&finishInit);
}

void ThreePhasePositionEstimator::finishInit(u2 const alpha) {
  if (!MLX90363::hasNewData(roll))
    return;

  // Disable alpha handler
  MLX90363::setAlphaHandler(nullptr);

  const auto phase = Lookup::AlphaToPhase(alpha);

  // lastMagPhase = phase.getMechanicalPosition();
  drivePhaseMagEstimate = u4(phase) << drivePhaseMagSubResolution;

  MLX90363::setAlphaHandler(&handleNewMagnetometerPositionReading);
}
