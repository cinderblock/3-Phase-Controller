/* 
 * File:   Config.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:12 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include <AVR++/bitTypes.h>

#if MotorID == Motor1
	#include "Config-Motor1.h"
#elif MotorID == Motor2
	#include "Config-Motor2.h"
#endif


using namespace AVR;

namespace Config {

	// static_assert(i2cSlaveAddress == 51, "Making rear motor");
	// static_assert(i2cSlaveAddress == 50, "Making front motor");
 
 u1 constexpr i2cBufferOutgoingDataSize = 14;
 u1 constexpr i2cBufferIncomingDataSize = 5;
 u1 constexpr i2cBufferOutgoingSize = i2cBufferOutgoingDataSize + 1;
 u1 constexpr i2cBufferIncomingSize = i2cBufferIncomingDataSize + 1;
 u1 constexpr i2cBufferIncomingBlocks = 2;
 u1 constexpr i2cExtraResponseMaxLength = 50;
};

#endif	/* CONFIG_H */

