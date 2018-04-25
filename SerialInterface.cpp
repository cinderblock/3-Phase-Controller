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

#include <TripleBuffer.cpp>

#include <CRC8.h>

libCameron::TripleBuffer<SerialInterface::Message, true> SerialInterface::incoming;
constexpr u1 SerialInterface::Message::header[headerLength];

u1 SerialInterface::Message::pos = 0;

void USART1_RX_vect() {
  SerialInterface::receiveByte();
}

void SerialInterface::init() {
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
  return crc(raw);
}

u1 SerialInterface::Message::crc(u1* block) {
  static CRC8 c;

  c.reset();

  for (u1 i = 0; i < length; i++) {
    c << *block++;
  }

  return c.getCRC();
}


template class libCameron::TripleBuffer<SerialInterface::Message, true>;
