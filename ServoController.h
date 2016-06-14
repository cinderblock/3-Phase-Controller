/**
 * The Servo controller for turnigy driver
 */

class ServoController {
private:

	enum class Mode:u1 {INIT, TOR, VEL, POS};

	static Mode currentMode;

	static s2 torqueCommand;
	static s2 velocityCommand;
	static s4 positionCommand;

	static u1 P;
	static u1 I;
	static u1 D;

	static u1 currentLimit;

public:
	static void init();

	static void update();

	static void setTorque(s2);
	static void setVelocity(s2);

	static void setZero();
	static void setPosition(s4);

	static void setCurrentLimit(u1);

	static void setP(u1);
	static void setI(u1);
	static void setD(u1);

	static void setTorqueEnable(bool);


};

