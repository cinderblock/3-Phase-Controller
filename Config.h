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
 u1 constexpr i2cBufferOutgoingSize = 11;
 u1 constexpr i2cBufferIncomingSize = 11;
}

#endif	/* CONFIG_H */

