/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SerialInterface.cpp
 * Author: Cameron
 * 
 * Created on March 26, 2018, 6:45 PM
 */

#include "SerialInterface.h"

#include <avr/io.h>

#include <CRC8.h>

using namespace ThreePhaseControllerNamespace;
using namespace libCameron;

libCameron::TripleBuffer<SerialInterface::Message, true> SerialInterface::incoming;
constexpr u1 SerialInterface::Message::header[headerLength];

u1 SerialInterface::Message::pos = 0;

void USART1_RX_vect() {
  ThreePhaseControllerNamespace::SerialInterface::receiveByte();
}

void SerialInterface::init() {
  // at F_CPU == 16MHz, this is 1MBaud
  UBRR1 = 0;

  // Set default
  UCSR1D = 0b00;

  // Set default
  UCSR1C = 0b00000110;

  // Set default
  UCSR1A = 0b00000000;

  // Make sure incoming buffer is ready to receive first block
  incoming.markNewestBuffer();

  // Enable transmitter, receiver, and rx interrupt
  UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1 << RXCIE1);
}

void SerialInterface::receiveByte() {
  incoming.getWriteBuffer()->feed(UDR1);
}

void SerialInterface::Message::feed(u1 b) {
  if (pos < headerLength) {
    if (header[pos] != b) {
      pos = 0;
      return;
    }
    pos++;
    return;
  }

  raw[pos - headerLength] = b;

  pos++;

  if (pos < headerLength + length) return;

  pos = 0;
  incoming.markNewestBuffer();
}

u1 SerialInterface::Message::checkCRC() {
  CRC8 c;

  auto data = raw;

  for (u1 i = 0; i < length; i++) {
    c << *data++;
  }

  return c.getCRC();
}

// Yes, we're including the .cpp
#include <TripleBuffer.cpp>
template class libCameron::TripleBuffer<SerialInterface::Message, true>;
