/* 
 * File:   FilebotInterface.h
 * Author: Cameron
 *
 * Created on April 10, 2017, 11:21 PM
 */

#ifndef FILEBOTINTERFACE_H
#define	FILEBOTINTERFACE_H

#include "Config.h"
#include "FilebotInterface/TwillBotInterface.h"

namespace ThreePhaseControllerNamespace {
 using CommInterface = FileBotInterface::TwillBotInterface<Config::i2cSlaveAddress, Config::i2cBufferIncomingSize, Config::i2cBufferOutgoingSize, Config::i2cBufferIncomingBlocks, 3>;
 
};

#endif	/* FILEBOTINTERFACE_H */

