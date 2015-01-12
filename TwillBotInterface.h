/* 
 * File:   TwillBotInterface.h
 * Author: Cameron
 *
 * Created on January 12, 2015, 12:33 AM
 */

#ifndef TWILLBOTINTERFACE_H
#define	TWILLBOTINTERFACE_H

#include "Config.h"

class TwillBotInterface {
 static constexpr b7 address = Config::I2CslaveAddress;
 
 
public:
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

