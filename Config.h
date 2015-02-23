/* 
 * File:   Config.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:12 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include "bitTypes.h"

namespace Config {
 b7 constexpr i2cSlaveAddress = 0b1100000;
 u1 constexpr i2cBufferSize = 10;
}

#endif	/* CONFIG_H */

