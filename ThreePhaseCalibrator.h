
#ifndef THREEPHASECALIBRATOR_H
#define	THREEPHASECALIBRATOR_H

#include "Config.h"

class ThreePhaseCalibrator{
	static u2 position;
	static u1 magRoll;

	static u2 constexpr MAX = 0x300 - 1;

public:
	static void init();
	static void goTo(u2);
	static void advance();

	inline static u2 getPhasePos(){return position;};
};


#endif	/* THREEPHASECALIBRATOR_H */
