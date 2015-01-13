/* 
 * File:   TwillBotInterface.h
 * Author: Cameron
 *
 * Created on January 12, 2015, 12:33 AM
 */

#ifndef TWILLBOTINTERFACE_H
#define	TWILLBOTINTERFACE_H

#include <avr/interrupt.h>
#include "Config.h"

ISR(TWI_vect);

class TwillBotInterface {
 static constexpr b7 address = Config::I2CslaveAddress;
 friend void TWI_vect();
 
public:
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

