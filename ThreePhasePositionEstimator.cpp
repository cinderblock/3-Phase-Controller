
#include "ThreePhasePositionEstimator.h"
// #include "Debug.h"
#include <util/atomic.h>
#include "DriverConstants.h"
#include "ServoController.h"
#include "Config.h"
#include "LookupTable.h"
// #include "Board.h"
// #include <iostream>
// #include <cmath> 
// #include "common.h"

using namespace std;
using namespace AVR;
using namespace ThreePhaseControllerNamespace;

u4 ThreePhasePositionEstimator::drivePhase;
u2 ThreePhasePositionEstimator::lastMecPha;
s2 ThreePhasePositionEstimator::driveVelocity;
u1 ThreePhasePositionEstimator::adjustVal;
u1 ThreePhasePositionEstimator::phaseAdvanceRatio;
s4 ThreePhasePositionEstimator::phaseAdvanceAmount;

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

	u4 ph = drivePhase;
	ph += driveVelocity;

	const bool forward = driveVelocity > 0;

	static const u4 MAX = ((u4)DriverConstants::StepsPerCycle) << DriverConstants::drivePhaseValueShift;

	// Check if ph(ase) value is out of range
	limit(ph, MAX, forward);

	// Store new drivePhase
	drivePhase = ph;

	// Adjust output for velocity lag
	ph += phaseAdvanceAmount;

	// Check if ph(ase) value is out of range again
	limit(ph, MAX, forward);

	// if(ph>>DriverConstants::drivePhaseValueShift > DriverConstants::MaskForPhase) Board::LED.on();
	return (ph >> DriverConstants::drivePhaseValueShift);
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

  const auto position = Lookup::AlphaToPhase(alpha);

	u2 mechanicalPhase = position.getMechanicalPosition();

	//find distance traveled in phase
	s2 mechChange = mechanicalPhase - lastMecPha;

	//TODO ensure we are not wrapping in the wrong direction due to high speeds
	if (mechChange > (s2)(DriverConstants::StepsPerRotation/2)){
		mechChange = mechChange - ((s2)DriverConstants::StepsPerRotation);
	}
	else if (mechChange < -((s2)DriverConstants::StepsPerRotation/2)){
		mechChange = mechChange + ((s2)DriverConstants::StepsPerRotation);
	}

	s4 tempVelocity = nextVelocity(mechChange);
	s4 tempPhaseAdvance = tempVelocity * phaseAdvanceRatio;

	// static const s4 MAX = ((u4)DriverConstants::StepsPerCycle) << DriverConstants::drivePhaseValueShift;

	// if(tempVelocity > 0){
	// 	if (tempPhaseAdvance > MAX) tempPhaseAdvance %= MAX;
	// }
	// else{
	// 	if (-tempPhaseAdvance > MAX) tempPhaseAdvance %= MAX;
	// }
	// tempPhaseAdvance %= MAX;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		driveVelocity = tempVelocity;
		drivePhase = u4(position.getPhasePosition()) << DriverConstants::drivePhaseValueShift;
		phaseAdvanceAmount = tempPhaseAdvance;
	}

	// Save the most recent magnetic position
	lastMecPha = mechanicalPhase;
}

s4 ThreePhasePositionEstimator::nextVelocity(s2 measuredMechChange) {

	s4 tempVelocity = driveVelocity;

	const s2 predictedPhaseChange = (tempVelocity * ((s1)DriverConstants::PredictsPerValue)) >> DriverConstants::drivePhaseValueShift;

	if (measuredMechChange > predictedPhaseChange) {
		tempVelocity += adjustVal;
	} else if (measuredMechChange < predictedPhaseChange) {
		tempVelocity -= adjustVal;
	}
	return tempVelocity;
}

void ThreePhasePositionEstimator::init(MotorPosition phase) {

	driveVelocity = 0;
	lastMecPha = phase.getMechanicalPosition();
	drivePhase = (u4)(lastMecPha & DriverConstants::MaskForPhase) << DriverConstants::drivePhaseValueShift;
	adjustVal = 5;
	phaseAdvanceRatio = Config::DefaultPhaseAdvance;
}
