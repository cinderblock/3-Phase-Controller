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

ISR(TWI_vect, ISR_NOBLOCK);

class TwillBotInterface {
 static constexpr b7 address = Config::i2cSlaveAddress;
 static constexpr bool generalCallEnable = true;
 friend void TWI_vect();
 inline static void isr();
 
 static constexpr u1 incomingBufferSize = 10;
 static constexpr u1 outgoingBufferSize = 10;

 static u1 bufferIndex;
 
 static bool generalCall;
 
 static bool newData;
 
 static bool firstByte;
 
 static u1 incomingBuffer[incomingBufferSize];
 static u1 outgoingBuffer[outgoingBufferSize];
public:
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

