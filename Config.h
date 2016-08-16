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
 bool constexpr forward = false;
 u1 constexpr i2cBufferOutgoingDataSize = 14;
 u1 constexpr i2cBufferIncomingDataSize = 10;
 u1 constexpr i2cBufferOutgoingSize = i2cBufferOutgoingDataSize + 1;
 u1 constexpr i2cBufferIncomingSize = i2cBufferIncomingDataSize + 1;
 u1 constexpr i2cBufferIncomingBlocks = 2;
 u1 constexpr i2cExtraResponseMaxLength = 50;
}

#endif	/* CONFIG_H */

