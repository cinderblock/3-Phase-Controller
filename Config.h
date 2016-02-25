/* 
 * File:   Config.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:12 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include <AVR++/bitTypes.h>

using namespace AVR;

namespace Config {
 b7 constexpr i2cSlaveAddress = 50;
 u1 constexpr i2cBufferOutgoingSize = 10;
 u1 constexpr i2cBufferIncomingSize = 10;
}

#endif	/* CONFIG_H */

