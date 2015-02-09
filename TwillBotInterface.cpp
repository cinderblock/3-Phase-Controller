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
u1 TwillBotInterface::outgoingBuffer[];

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
 
 if (s == Status::SL_SLA_W_ACK) {
  // Don't need to do anything. Everything should be ready for receiving data
  generalCall = false;
 }
 if (s == Status::SL_SLA_W_ACK_ArbitrationLost) {
  // This would only happen if we were a master while trying to send and then
  // were told to be a slave
 }
 if (s == Status::SL_General_ACK) {
  generalCall = true;
  // Return nack for now
  nack = true;
 }
 if (s == Status::SL_General_ACK_ArbitrationLost) {
  // This would only happen if we were a master while trying to send and then
  // some other master tried to go to the general call
  generalCall = false;
 }
 if (s == Status::SL_DATA_RX_ACK) {
  incomingBuffer[incomingIndex++] = *DR;
  if (incomingIndex == incomingLength)
   nack = true;
 }
 if (s == Status::SL_DATA_RX_NACK) {
  incomingBuffer[incomingIndex] = *DR;
 }
 if (s == Status::SL_General_DATA_RX_ACK) {
  nack = true;
  // TODO: handle general call
 }
 if (s == Status::SL_General_DATA_RX_NACK) {
  // TODO: handle general call
 }
 if (s == Status::SL_STOP_RESTART) {
  
 }
 
 if (s == Status::SL_SLA_R_ACK) {
  *DR = outgoingBuffer[outgoingIndex++];
  if (outgoingIndex == outgoingLength)
   nack = true;
 }
 if (s == Status::SL_SLA_R_ACK_ArbitrationLost) {
  // Not a master. Won't get here.
 }
 if (s == Status::SL_DATA_TX_ACK) {
  *DR = outgoingBuffer[outgoingIndex++];
  if (outgoingIndex == outgoingLength)
   nack = true;
 }
 if (s == Status::SL_DATA_TX_NACK) {
  *DR = outgoingBuffer[outgoingIndex];
 }
 if (s == Status::SL_DATA_TX_ACK_Done) {
  nack = true;
 }
 
// CR->EnableAcknowledge = nack;
// CR->InterruptFlag = 1;
 CR->byte = nack ? 0b10000101 : 0b11000101;
}

