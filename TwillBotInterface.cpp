/* 
 * File:   TwillBotInterface.cpp
 * Author: Cameron
 * 
 * Created on January 12, 2015, 12:33 AM
 */

#include "TwillBotInterface.h"
#include "I2C.h"

using namespace AVR::I2C;

u1 TwillBotInterface::incomingBuffer[];
u1 TwillBotInterface::outgoingBuffer[] = {0,1,2,3,4,5,6,7,8,9};

u1 TwillBotInterface::incomingIndex;
u1 TwillBotInterface::outgoingIndex;

bool TwillBotInterface::generalCall;

void TWI_vect() {
 TwillBotInterface::isr();
}

void TwillBotInterface::init() {
 outgoingIndex = 0;
 incomingIndex = 0;

 *(ARt*const)AR = {generalCallEnable,address};
 *AMR = 0;
 
 CR->byte = 0b01000101;
}

void TwillBotInterface::isr() {
 Status s = AVR::I2C::getStatus();
 bool nack = false;
 
 if (s == Status::SlaveWriteAck) {
  // Don't need to do anything. Everything should be ready for receiving data
  generalCall = false;
 }
 if (s == Status::SlaveWriteAckMasterLost) {
  // This would only happen if we were a master while trying to send and then
  // were told to be a slave
 }
 if (s == Status::SlaveGeneralAck) {
  generalCall = true;
  // Return nack for now
  nack = true;
 }
 if (s == Status::SlaveGeneralAckMasterLost) {
  // This would only happen if we were a master while trying to send and then
  // some other master tried to go to the general call
  generalCall = false;
 }
 if (s == Status::SlaveDataReceivedAck) {
  incomingBuffer[incomingIndex++] = *DR;
  if (incomingIndex == incomingLength)
   nack = true;
 }
 if (s == Status::SlaveDataReceivedNack) {
  incomingBuffer[incomingIndex] = *DR;
 }
 if (s == Status::SlaveGeneralDataReceivedAck) {
  nack = true;
  // TODO: handle general call
 }
 if (s == Status::SlaveGeneralDataReceivedNack) {
  // TODO: handle general call
 }
 if (s == Status::SlaveStopped) {
  
 }
 
 if (s == Status::SlaveReadAck) {
  *DR = outgoingBuffer[outgoingIndex++];
  if (outgoingIndex == outgoingLength)
   nack = true;
 }
 if (s == Status::SlaveReadAckMasterLost) {
  // Not a master. Won't get here.
 }
 if (s == Status::SlaveDataTransmittedAck) {
  *DR = outgoingBuffer[outgoingIndex++];
  if (outgoingIndex == outgoingLength)
   nack = true;
 }
 if (s == Status::SlaveDataTransmittedNack) {
  *DR = outgoingBuffer[outgoingIndex];
 }
 if (s == Status::SlaveDataTransmittedAckDone) {
  nack = true;
 }
 
// CR->EnableAcknowledge = nack;
// CR->InterruptFlag = 1;
 CR->byte = nack ? 0b10000101 : 0b11000101;
}

