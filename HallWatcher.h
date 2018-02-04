/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HallWatcher.h
 * Author: Cameron
 *
 * Created on October 12, 2017, 1:53 PM
 */

#ifndef HALLWATCHER_H
#define HALLWATCHER_H

#include <avr/interrupt.h>

#include "Board.h"

ISR(INT0_vect); // H1
ISR(INT1_vect); // H2
ISR(PCINT0_vect); // H3 (PCINT4 in group PCINT0)

namespace ThreePhaseControllerNamespace {

  using namespace AVR;

  class HallWatcher {
    using H1 = Board::H1; // INT0
    using H2 = Board::H2; // INT1
    using H3 = Board::H3; // PCINT4

    static u1 state;

    static inline void checkH1() {
      if (H1::isActive())
        state |= 1 << 0;
      else
        state &= ~(1 << 0);
    }

    static inline void checkH2() {
      if (H2::isActive())
        state |= 1 << 1;
      else
        state &= ~(1 << 1);
    }

    static inline void checkH3() {
      if (H3::isActive())
        state |= 1 << 2;
      else
        state &= ~(1 << 2);
    }

    friend void ::INT0_vect();
    friend void ::INT1_vect();
    friend void ::PCINT0_vect();

  public:
    static void init();

    inline static u1 getState() {
      return state;
    }
  private:

  };

};

#endif /* HALLWATCHER_H */

