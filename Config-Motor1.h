
#include <AVR++/bitTypes.h>



namespace ThreePhaseControllerNamespace {

using namespace AVR;

namespace Config {
	b7 constexpr i2cSlaveAddress = 50;
	bool constexpr forward = true;
	u1 constexpr MaxTorque = 40;
	u1 constexpr DefaultPhaseAdvance = 40;
}

};
