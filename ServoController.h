/**
 * The Servo controller for turnigy driver
 */

#ifndef SERVOCONTROLLER_H
#define	SERVOCONTROLLER_H

#include <AVR++/basicTypes.h>

using namespace AVR;

class ServoController {
private:

	enum class Mode:u1 {INIT, TOR, VEL, POS};

	static Mode currentMode;

	static s2 torqueCommand;
	static s4 torqueShiftCommand;
	static s2 velocityCommand;
	static s4 positionCommand;

	static s4 shiftingLimit;

	static u2 velocityAdjust;

	static u1 P;
	static u1 I;
	static u1 D;

	static u1 currentLimit;

	static const u1 DeadBand = 50;

public:
	static void init();

	static void update();

	static void setTorque(s2);
	static void setVelocity(s2);
	inline static s2 getVelocityCommand(){return velocityCommand;};

	static void setZero();
	static void setPosition(s4);

	static void setCurrentLimit(u1);

	static inline void setP(u1 p){P = p;};
	static inline void setI(u1 i){I = i;};
	static inline void setD(u1 d){D = d;};

	static void setTorqueEnable(bool);


};

#endif	/* SERVOCONTROLLER_H */

