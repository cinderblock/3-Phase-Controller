
#include "ThreePhasePositionEstimator.h"
// #include "Debug.h"
#include "Config.h"
#include "DriverConstants.h"
#include "HallWatcher.h"
#include "LookupTable.h"
#include "MLX90363.h"
#include "ServoController.h"
#include <util/atomic.h>
#include <util/delay.h>

// #include "Board.h"
// #include <iostream>
// #include <cmath>
// #include "common.h"

using namespace std;
using namespace AVR;
using namespace ThreePhaseControllerNamespace;

ThreePhaseDriver::PhasePosition ThreePhasePositionEstimator::drivePhaseHallEstimate;

u4 ThreePhasePositionEstimator::drivePhaseMagEstimate;
u2 ThreePhasePositionEstimator::lastMagPhase;

// Assume we start at 0 velocity
s2 ThreePhasePositionEstimator::driveVelocityMagEstimate = 0;

u1 ThreePhasePositionEstimator::phaseAdvanceMagRatio = Config::DefaultPhaseAdvance;
s4 ThreePhasePositionEstimator::phaseAdvanceMagCachedAmount = 0;
u1 ThreePhasePositionEstimator::mlxReadingsStarted = 0;
u1 ThreePhasePositionEstimator::qualityMagEstimate = 0;

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

ThreePhaseDriver::PhasePosition ThreePhasePositionEstimator::advance() {
  HallWatcher::checkH1();
  HallWatcher::checkH2();
  HallWatcher::checkH3();
  getAndProcessNewHallState();
  return drivePhaseHallEstimate;

  // Start at cyclesPWMPerMLX so that we have a whole period before the second
  // reading. The first reading was started in init();
  // If cyclesPWMPerMLX is 1, will try to communicate over SPI every time this function is called.
  u1 static mlxPeriodCounter = cyclesPWMPerMLX;

  // Automatically start MLX communications every few ticks
  if (!--mlxPeriodCounter) {
    MLX90363::startTransmitting();
    mlxReadingsStarted++;
    mlxPeriodCounter = cyclesPWMPerMLX;
  }

  u4 ph = drivePhaseMagEstimate;
  ph += driveVelocityMagEstimate;

  const bool forward = driveVelocityMagEstimate > 0;

  static constexpr u4 MAX = ((u4)DriverConstants::StepsPerCycle) << drivePhaseMagSubResolution;

  // Check if ph(ase) value is out of range
  limit(ph, MAX, forward);

  // Store new drivePhase
  drivePhaseMagEstimate = ph;

  // Adjust output for velocity lag
  // ph += phaseAdvanceMagCachedAmount;

  // Check if ph(ase) value is out of range again
  // limit(ph, MAX, forward);

  // If we're going fast, use Hall position readings directly
  if (qualityMagEstimate < 100) {
    return drivePhaseHallEstimate;
  }

  return (ph >> drivePhaseMagSubResolution);
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

void ThreePhasePositionEstimator::handleNewPositionReading(u2 alpha) {
  // Here, we are receiving a new position reading from the magnetometer.
  // We need to take this new reading, update our running estimates, and be
  // done. This is called by the magnetometer software after a new reading has
  // been received and validated with CRC. We will need to handle issues like
  // missing a count.

  // While this is technically called from inside an ISR, interrupts have been
  // re-enabled. Therefore, main() code will be paused if this is running.

  // static u1 tick = 0;

  // Debug::SOUT
  //         << Debug::Printer::Special::Start
  //         << tick++
  //         << reading
  //         << Debug::Printer::Special::End;

  u1 const numberOfCycles = mlxReadingsStarted;
  mlxReadingsStarted = 0;

  const auto position = Lookup::AlphaToPhase(alpha);

  u2 mechanicalPhase = position.getMechanicalPosition();

  // Find distance traveled in phase
  s2 mechChange = mechanicalPhase - lastMagPhase;

  // TODO: ensure we are not wrapping in the wrong direction due to high speeds
  if (mechChange > (s2)(DriverConstants::StepsPerRotation / 2)) {
    mechChange = mechChange - ((s2)DriverConstants::StepsPerRotation);
  } else if (mechChange < -((s2)DriverConstants::StepsPerRotation / 2)) {
    mechChange = mechChange + ((s2)DriverConstants::StepsPerRotation);
  }

  // Now we figure out how much to adjust our velocity estimate by to get back
  // in lock

  s4 tempVelocity = driveVelocityMagEstimate;

  // Here, instead of measuring how far we went and dividing by the number of
  // steps it took to get here, we predict how far we would have gone if our
  // estimate was accurate and then directly compare that to the actual
  // mechanical distance traveled. If we're too fast, adjust down. If we're too
  // slow, adjust up. Also handle if we missed data from the MLX because of CRC
  // error or something
  const s2 predictedPhaseChange =
      (tempVelocity * DriverConstants::PredictsPerValue * numberOfCycles) >> drivePhaseMagSubResolution;

  const s2 phaseError = mechChange - predictedPhaseChange;

  tempVelocity += phaseError / DriverConstants::PredictsPerValue;

  //	if (phaseError > 0) {
  //		tempVelocity += adjustVal;
  //	} else if (phaseError < 0) {
  //		tempVelocity -= adjustVal;
  //	}

  // Since we know these readings are old, do a simple approximation of the
  // needed phase advance to compensate for the delayed readings and store this
  // value for direct usage in advance() (NOTE: This is different from the phase
  // advance achieved with FOC)
  s4 tempPhaseAdvance = tempVelocity * phaseAdvanceMagRatio;

  // These values are accessed by other parts on interrupts. Turn off interrupts
  // so that the prediction steps are delayed until the new values are copied
  // over.
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    driveVelocityMagEstimate = tempVelocity;
    drivePhaseMagEstimate = u4(position.getPhasePosition()) << drivePhaseMagSubResolution;
    phaseAdvanceMagCachedAmount = tempPhaseAdvance;
  }

  // Save the most recent magnetic position
  lastMagPhase = mechanicalPhase;
}

void ThreePhasePositionEstimator::init() {
  // Just use halls for now
  HallWatcher::init();
  HallWatcher::setStateChangeReceiver(&getAndProcessNewHallState);
  return;

  MLX90363::init();
  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  auto magRoll = MLX90363::getRoll();

  do {
    MLX90363::startTransmitting();
    // Delay long enough to guarantee data is ready
    _delay_ms(2);

    // Loop until we actually receive real data
  } while (!MLX90363::hasNewData(magRoll));

  MLX90363::setAlphaHandler(&handleNewPositionReading);

  const auto phase = Lookup::AlphaToPhase(MLX90363::getAlpha());

  // Set up the hall sensor interrupts
  HallWatcher::init();
  HallWatcher::setStateChangeReceiver(&getAndProcessNewHallState);

  lastMagPhase = phase.getMechanicalPosition();
  drivePhaseMagEstimate = (u4)(phase.getPhasePosition()) << drivePhaseMagSubResolution;
}
