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

 /**
  * The buffer that SLA+W writes to and that the local software reads from
  * 
  * Since we're reading from the main loop, we need to enable interrupts to protect
  * the buffer's state only on the reading side.
  */ 
 static TripleBuffer<incomingBufferSize, true> incomingBuffer;

 /**
  * The buffer that SLA+R reads from and that the local software write to
  * 
  * Since we're writing from the main loop, we need to enable interrupts to protect
  * the buffer's state only on the writing side.
  */ 
 static TripleBuffer<outgoingBufferSize, false> outgoingBuffer;
public:
 
 /**
  * Check if the master has sent a new block
  * @return 
  */
 inline static bool hasReceivedBlock() {return incomingBuffer.isNewData();}
 
 /**
  * If the master has send a new block, we can reserve it now
  */
 inline static void reserveNextReadBuffer() {incomingBuffer.reserveNewestBufferForReading();}

 /**
  * Once we've reserved out read buffer, we can grab it as many times as we like
  * @return 
  */
 inline static u1 * getIncomingReadBuffer() {return incomingBuffer.getReadBuffer();}

 /**
  * Get the current write block that we're preparing for the master
  * 
  * Release when new data is fully written
  * @return 
  */
 inline static u1 * getOutgoingWriteBuffer() {return outgoingBuffer.getWriteBuffer();}

 /**
  * Mark the current block of data as ready to send to the master
  */
 inline static void releaseNextWriteBuffer() {outgoingBuffer.markNewestBuffer();}
 
 static void init();
 
 

};

#endif	/* TWILLBOTINTERFACE_H */

