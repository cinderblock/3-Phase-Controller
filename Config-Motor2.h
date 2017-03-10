
#include <AVR++/bitTypes.h>
using namespace AVR;

namespace Config {
	b7 constexpr i2cSlaveAddress = 51;
	bool constexpr forward = false;
	u1 constexpr MaxTorque = 30;
	u1 constexpr DefaultPhaseAdvance = 50;
}
