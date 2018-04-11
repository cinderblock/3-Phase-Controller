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
    friend class SerialInterface;
    static constexpr u1 headerLength = 3;
    static constexpr u1 header[headerLength] = {0xff, 0xfe, 0xfe};

    /**
     * fix length of message, including crc
     */
    static constexpr u1 length = 2;


    static u1 crc(u1 *);

    /**
     * Track the current position we're writing to. Static because we never
     * write to this from multiple places.
     */
    static u1 pos;

    static u1 scratch[headerLength];

    /**
     * Internal block of bytes we're wrapping around
     */
    u1 raw[length];

    u1 checkCRC();

    /**
     * Feed a new byte to the message parser
     * @param b the byte to parse
     * @return true if a complete message has been received and validated
     */
    bool feed(u1 b);
  public:
    inline s2 getCommand() const {
      return 2 * s1(raw[0]);
    }
  };

public:
  inline static bool isMessageReady() {
    return incoming.isNewData();
  }

  inline static Message const * getMessage() {
    return incoming.getReadBuffer();
  }

  inline static void receiveMessage() {
    incoming.reserveNewestBufferForReading();
  }

private:
  static TripleBuffer<Message, true> incoming;

};

#endif /* SERIALINTERFACE_H */

