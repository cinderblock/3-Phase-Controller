
#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "Predictor.h"
#include <AVR++/basicTypes.h>
#include "DriverConstants.h"

using namespace AVR;

class Predictor{
	// static constexpr u1 PredictsPerValue = 40;

	static u2 lastMecPha;
	static u4 drivePhase;
	static s2 driveVelocity;
	static s2 lastMechChange;
	static u2 lastReading;
	static u1 adjustVal;
	static u1 phaseAdvanceRatio;

	inline static u2 getMechPhase(u2 phase){return (phase & DriverConstants::BitsForPhase) + (phase >> 12) * DriverConstants::StepsPerCycle;};

public:
	// static u1 ratio;
	// static u2 adjustVal;

	static void freshPhase(u2 phase);
	static void init(u2 initialPhase);

	static u2 predict();
	static s4 nextVelocity(s2);

	inline static u4 getPredictedPosition(){return drivePhase;}
	inline static u2 getMeasuredPosition() {return lastMecPha;}
	inline static s2 getVelocity()         {return driveVelocity;}

	inline static u1 getAdjustVal(){return adjustVal;}
	inline static u1 getPhaseAdvanceRatio(){return phaseAdvanceRatio;}
	inline static void setAdjustVal(u1 val){adjustVal = val;}
	inline static void setPhaseAdvanceRatio(u1 val){phaseAdvanceRatio = val;}
};

#endif  /* PREDICTOR_H */
