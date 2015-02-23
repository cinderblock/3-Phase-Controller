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
#include "TripleBuffer.h"

ISR(TWI_vect, ISR_NOBLOCK);

class TwillBotInterface {
 static constexpr b7 address = Config::i2cSlaveAddress;
 static constexpr bool generalCallEnable = true;
 friend void TWI_vect();
 inline static void handleNextI2CByte();
 
 static constexpr u1 incomingBufferSize = Config::i2cBufferIncomingSize;
 static constexpr u1 outgoingBufferSize = Config::i2cBufferOutgoingSize;

 static u1 bufferIndex;
 
 static bool generalCall;
 
 static bool newData;
 
 static bool firstByte;

 /**
  * The buffer that SLA+W writes to and that the local software reads from
  */ 
 static TripleBuffer<incomingBufferSize, true> incomingBuffer;

 /**
  * The buffer that SLA+R reads from and that the local software write to
  */ 
 static TripleBuffer<outgoingBufferSize, false> outgoingBuffer;

 inline static u1 * getOutgoingReadBuffer() {return outgoingBuffer.getReadBuffer();}
 
 inline static u1 * getIncomingWriteBuffer() {return incomingBuffer.getWriteBuffer();}
 
 inline static u1 * getCurrentOutgoingReadBuffer() {return outgoingBuffer.getCurrentReadBuffer();}
 
 inline static u1 * getCurrentIncomingWriteBuffer() {return incomingBuffer.getCurrentWriteBuffer();}
public:
 
 inline static u1 * getIncomingReadBuffer() {return incomingBuffer.getReadBuffer();}

 inline static u1 * getOutgoingWriteBuffer() {return outgoingBuffer.getWriteBuffer();}

 inline static u1 * getCurrentIncomingReadBuffer() {return incomingBuffer.getCurrentReadBuffer();}

 inline static u1 * getCurrentOutgoingWriteBuffer() {return outgoingBuffer.getCurrentWriteBuffer();}
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

