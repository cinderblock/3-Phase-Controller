
#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "ThreePhasePositionEstimator.h"
#include <AVR++/basicTypes.h>
#include "DriverConstants.h"

namespace ThreePhaseControllerNamespace {

using namespace AVR;

class ThreePhasePositionEstimator{
	static u2 lastMecPha;
	static u4 drivePhase;
	static s2 driveVelocity;
	static s2 lastMechChange;
	static u2 lastReading;
	static u1 adjustVal;
	static u1 phaseAdvanceRatio;
	static s4 phaseAdvanceAmount;
	static u2 lastPredicted;

	//gets the mechanical positon (in phase units, not magnetometer position)
	inline static u2 getMechPhase(u2 phase){return (phase & DriverConstants::MaskForPhase) + (phase >> 12) * DriverConstants::StepsPerCycle;};

public:
	//initalize predicotr with
	static void init(u2 initialPhase);

	//Receive a new magnetometer position
	//happens every 40 (ThreePhaseController::cyclesPWMPerMLX) pwm cycles
	static void freshPhase(u2 phase);

	//predict phase we are currently at based on last position reading and velocity precition
	//also updates our next prediction
	//happens every pwm cycles
	static u2 predictPhase();

	//takes in the delta distance travelled an updated extrapolated velocity
	static s4 nextVelocity(s2);

	//get the last predicted location
	inline static u2 getPredictedPosition(){return lastPredicted;};
	//get the phase advance ratio
	//estimation of delay from last magetometer reading and setting current phase
	inline static u1 getPhaseAdvanceRatio(){return phaseAdvanceRatio;};
	//setter
	inline static void setPhaseAdvanceRatio(u1 val){phaseAdvanceRatio = val;}
	//estimation of distance given there are delays in system (phaseAdvanceRatio * velocity)
	inline static s4 getPhaseAdvanceAmount(){return phaseAdvanceAmount;};
	//get the last measured position
	inline static u2 getMeasuredPosition(){return lastMecPha;};
	//get currently exrapolated velocity
	inline static s2 getVelocity(){return driveVelocity;};

	//get value the velocity may get shifted by per velocity update
	inline static u1 getAdjustVal(){return adjustVal;}
	inline static void setAdjustVal(u1 val){adjustVal = val;}
};

};

#endif  /* PREDICTOR_H */
