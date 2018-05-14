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

#include <stddef.h>
#include <AVR++/basicTypes.h>
#include <TripleBuffer.h>
#include <avr/interrupt.h>

#include <CRC8.h>

#include "Board.h"

ISR(USART1_RX_vect);

namespace ThreePhaseControllerNamespace {

using namespace AVR;
using namespace libCameron;

class SerialInterface {
  friend void ::USART1_RX_vect();
  inline static void receiveByte() __attribute__((always_inline, hot));
public:
  static void init();
  class Message {
    using crc = CRC8;
    friend class SerialInterface;
    static constexpr u1 headerLength = 3;
    static constexpr u1 header[headerLength] = {0xff, 0xfe, 0xfe};

    /**
     * Internal block of bytes we're wrapping around
     */
    typedef struct {
      s2 command;
    } block;

    /**
     * fix length of message, including crc
     */
    static constexpr size_t length = sizeof(block) + sizeof(crc);
    
    union {
      u1 raw[length];
      block data;
    };

    /**
     * Track the current position we're writing to. Static because we never
     * write to this from multiple places.
     */
    static u1 pos;

    u1 checkCRC();

    /**
     * Feed a new byte to the message parser
     * @param b the byte to parse
     */
    void feed(u1 b);
  public:
    inline s2 getCommand() const {
      return data.command;
    }
  };

public:
  inline static bool isMessageReady() {
    if (!incoming.isNewData()) return false;
    
    incoming.reserveNewestBufferForReading();

    return (incoming.getReadBuffer()->checkCRC() == 0);
  }

  inline static Message const * getMessage() {
    return isMessageReady() ? incoming.getReadBuffer() : nullptr;
  }

private:
  static TripleBuffer<Message, true> incoming;

};

}

#endif /* SERIALINTERFACE_H */
