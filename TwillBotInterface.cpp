/* 
 * File:   TwillBotInterface.cpp
 * Author: Cameron
 * 
 * Created on January 12, 2015, 12:33 AM
 */

#include "TwillBotInterface.h"
#include "I2C.h"

using namespace AVR::I2C;

TripleBuffer<TwillBotInterface::incomingBufferSize,  true> TwillBotInterface::incomingBuffer;
TripleBuffer<TwillBotInterface::outgoingBufferSize, false> TwillBotInterface::outgoingBuffer;

u1 TwillBotInterface::bufferIndex;

bool TwillBotInterface::generalCall;
bool TwillBotInterface::newData = false;
bool TwillBotInterface::firstByte = false;

void TWI_vect() {
 TwillBotInterface::handleNextI2CByte();
}

void TwillBotInterface::init() {
 AR->byte = (address << 1) | generalCallEnable;
 *AMR = 0;

 CR->byte = 0b01000101;
}

void TwillBotInterface::handleNextI2CByte() {
 const Status s = AVR::I2C::getStatus();
 bool ack = false;
 
 if (s == Status::SlaveWriteAcked) {
  // We've been told to accept incoming data

  firstByte = true;
  getIncomingWriteBuffer();
  
  ack = true;
  
  generalCall = false;
 }
 if (s == Status::SlaveWriteAckedMasterLost) {
  // This would only happen if we were a master while trying to send and then
  // were told to be a slave
 }
 if (s == Status::SlaveGeneralAcked) {
  generalCall = true;

  firstByte = true;
 }
 if (s == Status::SlaveGeneralAckedMasterLost) {
  // This would only happen if we were a master while trying to send and then
  // some other master tried to go to the general call
  //generalCall = false;
 }
 if (s == Status::SlaveDataReceivedAcked) {
  const u1 temp = *DR;
  if (firstByte) {
   bufferIndex = temp;
   firstByte = false;
  } else {
   getCurrentIncomingWriteBuffer()[bufferIndex] = temp;
   newData = true;
  }
  
  if (bufferIndex < incomingBufferSize)
   ack = true;
 }
 if (s == Status::SlaveDataReceivedNacked) {
  if (bufferIndex < incomingBufferSize) {
   incomingBuffer.getCurrentWriteBuffer()[bufferIndex] = *DR;
  }
  newData = true;
  ack = true;
 }
 if (s == Status::SlaveGeneralDataReceivedAcked) {
  // TODO: handle general call
 }
 if (s == Status::SlaveGeneralDataReceivedNacked) {
  // TODO: handle general call
 }
 if (s == Status::SlaveStopped) {
  ack = true;
 }
 
 if (s == Status::SlaveReadAcked || s == Status::SlaveDataTransmittedAcked) {
  *DR = outgoingBuffer.getCurrentReadBuffer()[bufferIndex++];
  if (bufferIndex < outgoingBufferSize)
   ack = true;
 }
 if (s == Status::SlaveDataTransmittedNacked) {
  // We told the hardware to stop transmitting after our previous byte and we
  // received a NACK from the master. Yay!
  
  // Start listening for our address again
  ack = true;
 }
 if (s == Status::SlaveDataTransmittedAckedDone) {
  // We told the hardware to stop transmitting after our previous byte and we
  // received an ACK from the master. Silly master, no more data for you!
  
  // Start listening for our address again
  ack = true;
 }
 
// CR->EnableAcknowledge = ack;
// CR->InterruptFlag = 1;
 CR->byte = ack ? 0b11000101 : 0b10000101;
}

