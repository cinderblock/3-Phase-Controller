
#ifndef DRIVECONST
#define DRIVECONST

#include <AVR++/basicTypes.h>

using namespace AVR;

class DriverConstants{
public:
	/**
	 * Number of predictions made between each magnetometer update
	 */
	static constexpr u1 PredictsPerValue = 40;
	
	/**
	 * shift phase integer for additional resolution
	 */
  static constexpr u1 drivePhaseValueShift = 8;

	/**
	 * Maximum torque provided by the motor
	 */
	static const u1 MaxTorque = 40;

	/**
	 * Internal granularity of sin wave for each phase
	 */
	static u2 constexpr StepsPerPhase = 256;
	
	/**
	 * One for each of A, B, and C.
	 */
	static u1 constexpr PhasesPerCycle = 3;
	
	/**
	 * One Cycle is one full commutation "revolution" of the motor. This is almost
	 * certainly not one actual revolution of the motor shaft.
	 */
	static u2 constexpr StepsPerCycle = StepsPerPhase * PhasesPerCycle;
	
};

#endif

