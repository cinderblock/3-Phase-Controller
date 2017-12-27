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

ISR(INT6_vect);
ISR(PCINT0_vect);

/*  this macro call of "ISR"
ISR(PCINT0_vect) {
  blah blah blah
}

...is the same as ...

void __vector_6() __attribute__((interrupt, used));
void __vector_6() {
  blah blah blah
}
*/

namespace ThreePhaseControllerNamespace {

  using namespace AVR;

  class HallWatcher {
    using H1 = Board::H1; // INT6
    using H2 = Board::H2; // PCINT7
    using H3 = Board::H3; // PCINT4

    // state is the three bits representing the three hall sensor state
    static volatile u1 state;

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

    friend void ::INT6_vect();
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
