
#include "ThreePhasePositionEstimator.h"
// #include "Debug.h"
#include <util/atomic.h>
#include "DriverConstants.h"
#include "ServoController.h"
#include "Config.h"
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
s2 ThreePhasePositionEstimator::lastMechChange;
u2 ThreePhasePositionEstimator::lastReading;
u1 ThreePhasePositionEstimator::adjustVal;
u1 ThreePhasePositionEstimator::phaseAdvanceRatio;
s4 ThreePhasePositionEstimator::phaseAdvanceAmount;
u2 ThreePhasePositionEstimator::lastPredicted;

inline static s2 constexpr abs(s2 num) {
	return num >= 0 ? num : -num;
}

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

u2 ThreePhasePositionEstimator::predictPhase() {

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
	lastPredicted = (ph >> DriverConstants::drivePhaseValueShift);
	return lastPredicted;
}

void ThreePhasePositionEstimator::freshPhase(u2 reading) {

	// static u1 tick = 0;

	// Debug::SOUT
	//         << Debug::Printer::Special::Start
	//         << tick++
	//         << reading
	//         << Debug::Printer::Special::End;


	u2 mechanicalPhase = getMechPhase(reading);

	//find distance travelled in phase
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
		drivePhase = u4(reading & DriverConstants::MaskForPhase) << DriverConstants::drivePhaseValueShift;
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

void ThreePhasePositionEstimator::init(u2 phase) {

	driveVelocity = 0;
	lastMecPha = getMechPhase(phase);//lookupAlphaToPhase(MLX90363::getAlpha());
	drivePhase = (u4)(lastMecPha & DriverConstants::MaskForPhase) << DriverConstants::drivePhaseValueShift;
	lastMechChange = 0;
	adjustVal = 5;
	phaseAdvanceRatio = Config::DefaultPhaseAdvance;
}
