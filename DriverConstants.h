
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
	 * The bits used to define an electrical revolution
	 */
	static u2 constexpr MaskForPhase = 0x3FF;

	/**
	 * One for each of A, B, and C.
	 */
	static u1 constexpr PhasesPerCycle = 3;
	
	/**
	 * One Cycle is one full commutation "revolution" of the motor. This is almost
	 * certainly not one actual revolution of the motor shaft.
	 */
	static u2 constexpr StepsPerCycle = StepsPerPhase * PhasesPerCycle;
	

	/**
	 * number of electrical (computational) rotation there are per mechanical one
	 */
	static u1 constexpr ElectricalPerMechanical = 7;
	
	/**
	 * Number of steps per mechanical revolution
	 */
	static u2 constexpr StepsPerRotation = StepsPerCycle * ElectricalPerMechanical;
	
	/**
	 * Maximum change in velocity
	 * TODO CURRENTLY A GUESS FIND OUT WHAT IT ACTUALLY IS
	 */
	static u2 constexpr MaxVelocityChange = StepsPerRotation / 3;
	
	/**
	 * Number of bits the magnetometer uses to give position
	 */
	static u1 constexpr MagnetometerBits = 14;

	/**
	 * Mask used to only grab magnetometer related bits from position
	 */
	static u2 constexpr MagnetometerBitsMask = (1<<MagnetometerBits)-1;

	/**
	 * Largest value the magnetometer can be
	 */
	static u2 constexpr MagnetometerMax = (1<<MagnetometerBits)-1;

};

#endif

