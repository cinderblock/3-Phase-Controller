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
 static constexpr bool generalCallEnable = true;
 friend void TWI_vect();
 inline static void isr();
 
 static constexpr u1 incomingBufferSize = 10;
 static constexpr u1 outgoingBufferSize = 10;
 
 static constexpr u1 incomingLength = incomingBufferSize;
 static constexpr u1 outgoingLength = outgoingBufferSize;
 
 static u1 incomingIndex;
 static u1 outgoingIndex;
 
 static bool generalCall;
 
 static u1 incomingBuffer[incomingBufferSize];
 static u1 outgoingBuffer[outgoingBufferSize];
public:
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

