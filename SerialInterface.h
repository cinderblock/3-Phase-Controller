/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SerialInterface.h
 * Author: Cameron
 *
 * Created on March 26, 2018, 6:45 PM
 */

#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <AVR++/basicTypes.h>
#include <TripleBuffer.h>
#include <avr/interrupt.h>

using namespace AVR;
using namespace libCameron;

ISR(USART1_RX_vect);

class SerialInterface {
  friend void USART1_RX_vect();
  static void receiveByte();
public:
  static void init();
  class Message {
    static constexpr u1 length = 5;
    u1 raw[length];
  };

private:
  static TripleBuffer<Message, true> incoming;

};

#endif /* SERIALINTERFACE_H */

