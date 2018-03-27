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

libCameron::TripleBuffer<SerialInterface::Message, true> SerialInterface::incoming;

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

  // Enable transmitter and Receiver
  UCSR1B = (1 << TXEN1) | (1 << RXEN1);

  // Make sure incoming buffer is ready to receive first block
  incoming.markNewestBuffer();

  // TODO: Enable interrupt
}

void SerialInterface::receiveByte() {
  // TODO: Implement interrupt
}


template class libCameron::TripleBuffer<SerialInterface::Message, true>;