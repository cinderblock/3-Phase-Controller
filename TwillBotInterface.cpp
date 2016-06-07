/* 
 * File:   TwillBotInterface.cpp
 * Author: Cameron
 * 
 * Created on January 12, 2015, 12:33 AM
 */

#include <AVR++/I2C.h>
#include <util/delay.h>

#include "TwillBotInterface.h"
#include "Board.h"
#include "Debug.h"

using namespace AVR::I2C;

TripleBuffer<TwillBotInterface::incomingBufferSize,  true> TwillBotInterface::incomingBuffer;
TripleBuffer<TwillBotInterface::outgoingBufferSize, false> TwillBotInterface::outgoingBuffer;

u1 TwillBotInterface::bufferIndex;

bool TwillBotInterface::generalCall;

void TWI_vect() {
 Debug::TwillBotInterface::ISR::enter();
 TwillBotInterface::InterruptServiceRoutine();
 Debug::TwillBotInterface::ISR::exit();
}

void TIMER0_COMPB_vect() {
 TwillBotInterface::timeout();
}


void TwillBotInterface::init() {
 TimerTimeout::init();

 AR->byte = (address << 1) | generalCallEnable;
 *AMR = 0;

// CR->byte = 0b01000101;
 CR->byte =
         1 << TWIE |
         1 << TWEN |
         1 << TWEA |
         1 << TWINT;
}

void TwillBotInterface::timeout() {
 //i2c timeout detected
 Debug::TwillBotInterface::timeout();
 TimerTimeout::stopBISR();

 //do something...
 const Status s = AVR::I2C::getStatus();
 
}

void TwillBotInterface::InterruptServiceRoutine() {
 // Turn off the TWI interrupt
 CR->byte = 1 << TWEN;
 
 // Enable global interrupts
 sei();
 
 TimerTimeout::stopBISR();

 TwillBotInterface::handleNextI2CByte();
}

void TwillBotInterface::handleNextI2CByte() {
 const Status s = AVR::I2C::getStatus();
 bool ack = false;
 
 if (s == Status::SlaveWriteAcked) {
  // We've been told to accept incoming data
  ack = true;
  bufferIndex = 0;
  
  generalCall = false;
 }
 
 if (s == Status::SlaveGeneralAcked) {
  generalCall = true;
  // Don't ACK any data after the general call
 }
 
 if (s == Status::SlaveDataReceivedAcked) {
  incomingBuffer.getWriteBuffer()[bufferIndex++] = *DR;
  
  // ACK until we can only fit one more byte
  if (bufferIndex < incomingBufferSize)
   ack = true;
 }
 
 if (s == Status::SlaveDataReceivedNacked) {
  if (bufferIndex < incomingBufferSize) {
   incomingBuffer.getWriteBuffer()[bufferIndex++] = *DR;
  }
  incomingBuffer.markNewestBuffer();

  // ACK the next SLA+R/W
  ack = true;
 }
 
 if (s == Status::SlaveGeneralDataReceivedAcked) {
  // TODO: handle general call
 }
 
 if (s == Status::SlaveGeneralDataReceivedNacked) {
  // TODO: handle general call
  
  // Make sure we leave with the interface enabled
  ack = true;
 }
 
 if (s == Status::SlaveStopped) {
  // This would happen if we ACKed the last byte received and the master stopped
  // In this case we're done. Mark the incoming buffer as newest.
  incomingBuffer.markNewestBuffer();
  // ACK the next SLA+R/W
  ack = true;
 }
 
 if (s == Status::SlaveReadAcked) {
  // We just entered SLA+R
  
  // Clear our buffer index
  bufferIndex = 0;
  
  // Only iff there is new data available should we send anything
  if (outgoingBuffer.isNewData()) {
   outgoingBuffer.reserveNewestBufferForReading();
   if (bufferIndex < outgoingBufferSize) {
    *DR = outgoingBuffer.getReadBuffer()[bufferIndex++];
    ack = true;
   }
  } else {
   // We don't have a block ready to send. Don't ACK and send a 0.
   *DR = 0;
  }

  _delay_us(rPiI2CClockStrechUS);
 }
 
 if (s == Status::SlaveDataTransmittedAcked) {
  _delay_us(rPiI2CClockStrechUS);

  // We told the AVR hardware to send a byte and we received an ACK as expected
  *DR = outgoingBuffer.getReadBuffer()[bufferIndex++];
  if (bufferIndex < outgoingBufferSize)
   ack = true;
 }
 
 if (s == Status::SlaveDataTransmittedNacked) {
  // We told the AVR hardware to stop transmitting after our previous byte and
  // we received a NACK from the master. Yay!
  
  // Start listening for our address again
  ack = true;
 }
 
 if (s == Status::SlaveDataTransmittedAckedDone) {
  // We told the AVR hardware to stop transmitting after our previous byte and
  // we received an ACK from the master. Silly master, no more data for you!
  *DR = 0xff;
  
  // Start listening for our address again. (ACK the next SLA+R/W)
  ack = true;
 }
 
 // Re-enable I2C interrupt while clearing the interrupt flag (by setting it)
// CR->EnableAcknowledge = ack;
// CR->InterruptFlag = 1;
// CR->byte = ack ? 0b11000101 : 0b10000101;
 CR->byte = ack ? (
         1 << TWIE |
         1 << TWEN |
         1 << TWEA |
         1 << TWINT
         ) : (
         1 << TWIE |
         1 << TWEN |
         0 << TWEA |
         1 << TWINT
         );
 
 TimerTimeout::startB(timeoutPeriod);
}
