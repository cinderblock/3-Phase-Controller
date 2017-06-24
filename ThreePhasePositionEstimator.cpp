
#include "ThreePhasePositionEstimator.h"
// #include "Debug.h"
#include <util/atomic.h>
#include "DriverConstants.h"
#include "ServoController.h"
#include "Config.h"
#include "LookupTable.h"
#include <MLX90363.h>
#include <util/delay.h>

// #include "Board.h"
// #include <iostream>
// #include <cmath> 
// #include "common.h"

using namespace std;
using namespace AVR;
using namespace ThreePhaseControllerNamespace;

u4 ThreePhasePositionEstimator::drivePhase;
u2 ThreePhasePositionEstimator::lastMecPha;

// Assume we start at 0 velocity
s2 ThreePhasePositionEstimator::driveVelocity = 0;


u1 ThreePhasePositionEstimator::adjustVal;
u1 ThreePhasePositionEstimator::phaseAdvanceRatio;
s4 ThreePhasePositionEstimator::phaseAdvanceAmount;
u1 ThreePhasePositionEstimator::mlxReadingsStarted = 0;

inline static s2 constexpr abs(s2 num) {
	return num >= 0 ? num : -num;
}

inline static void limit(u4& value, u4 MAX, bool forward) __attribute__((hot));
inline static void limit(u4& value, u4 MAX, bool forward) {
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
  // Start at cyclesPWMPerMLX so that we have a whole period before the second reading.
  // The first reading was started in init();
  u1 static mlx = cyclesPWMPerMLX;

  // Automatically start MLX communications every few ticks
  if (!--mlx) {
    MLX90363::startTransmitting();
    mlxReadingsStarted++;
    mlx = cyclesPWMPerMLX;
  }

	u4 ph = drivePhase;
	ph += driveVelocity;

	const bool forward = driveVelocity > 0;

	static constexpr u4 MAX = ((u4)DriverConstants::StepsPerCycle) << predictionResolutionShift;

	// Check if ph(ase) value is out of range
	limit(ph, MAX, forward);

	// Store new drivePhase
	drivePhase = ph;

	// Adjust output for velocity lag
	ph += phaseAdvanceAmount;

	// Check if ph(ase) value is out of range again
	limit(ph, MAX, forward);

	// if(ph>>predictionResolutionShift > DriverConstants::MaskForPhase) Board::LED.on();
	return (ph >> predictionResolutionShift);
}

void ThreePhasePositionEstimator::handleNewPositionReading(u2 alpha) {
  // Here, we are receiving a new position reading from the magnetometer.
  // We need to take this new reading, update our running estimates, and be done.
  // This is called by the magnetometer software after a new reading has been received
  // and validated with CRC. We will need to handle issues like missing a count.

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
	s2 mechChange = mechanicalPhase - lastMecPha;

	// TODO: ensure we are not wrapping in the wrong direction due to high speeds
	if (mechChange > (s2)(DriverConstants::StepsPerRotation/2)){
		mechChange = mechChange - ((s2)DriverConstants::StepsPerRotation);
	}
	else if (mechChange < -((s2)DriverConstants::StepsPerRotation/2)){
		mechChange = mechChange + ((s2)DriverConstants::StepsPerRotation);
	}

  // Now we figure out how much to adjust our velocity estimate by to get back in lock

	s4 tempVelocity = driveVelocity;

  // Here, instead of measuring how far we went and dividing by the number of steps
  // it took to get here, we predict how far we would have gone if our estimate was
  // accurate and then directly compare that to the actual mechanical distance traveled.
  // If we're too fast, adjust down. If we're too slow, adjust up.
  // Also handle if we missed data from the MLX because of CRC error or something
	const s2 predictedPhaseChange = (tempVelocity * DriverConstants::PredictsPerValue * numberOfCycles) >> predictionResolutionShift;

	if (mechChange > predictedPhaseChange) {
		tempVelocity += adjustVal;
	} else if (mechChange < predictedPhaseChange) {
		tempVelocity -= adjustVal;
	}

  // Since we know these readings are old, do a simple approximation of the needed
  // phase advance to compensate for the delayed readings and store this value for
  // direct usage in advance()
  // (NOTE: This is different from the phase advance achieved with FOC)
	s4 tempPhaseAdvance = tempVelocity * phaseAdvanceRatio;

  // These values are accessed by other parts on interrupts. Turn off interrupts
  // so that the prediction steps are delayed until the new values are copied over.
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		driveVelocity = tempVelocity;
		drivePhase = u4(position.getPhasePosition()) << predictionResolutionShift;
		phaseAdvanceAmount = tempPhaseAdvance;
	}

	// Save the most recent magnetic position
	lastMecPha = mechanicalPhase;
}

void ThreePhasePositionEstimator::init() {
  MLX90363::init();
  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  auto magRoll = MLX90363::getRoll();

  do {
    MLX90363::startTransmitting();
    // Delay long enough to guarantee data is ready
    _delay_ms(2);

    // Loop until we actually receive real data
  } while (!MLX90363::hasNewData(magRoll));

  const auto phase = Lookup::AlphaToPhase(MLX90363::getAlpha());

	driveVelocity = 0;
	lastMecPha = phase.getMechanicalPosition();
	drivePhase = (u4)(phase.getPhasePosition()) << predictionResolutionShift;
	adjustVal = 5;
	phaseAdvanceRatio = Config::DefaultPhaseAdvance;
}
