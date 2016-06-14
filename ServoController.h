/**
 * The Servo controller for turnigy driver
 */

class ServoController {
private:
	

public:
	static void init();

	static void setTorque(s2);
	static void setVelocity(s2);
	static void setPosition(s4);

	static void setCurrent(u1);

	static void setP(u1);
	static void setI(u1);
	static void setD(u1);

	static void setTorqueEnable(u1);

	
};

