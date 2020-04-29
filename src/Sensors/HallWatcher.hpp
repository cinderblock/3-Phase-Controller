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

#include "../System/Board.hpp"

#ifdef QUANTUM_DRIVE
ISR(INT0_vect);   // H1
ISR(INT1_vect);   // H2
ISR(PCINT0_vect); // H3 (PCINT4 in group PCINT0)
#endif

#ifdef BED_CONTROLLER
ISR(INT6_vect);   // H1
ISR(PCINT0_vect); // H2 & H3 (PCINT7 & PCINT4 respectively)
#endif

namespace ThreePhaseControllerNamespace {

using namespace AVR;

class HallWatcher {
  using H1 = Board::H1;
  using H2 = Board::H2;
  using H3 = Board::H3;

  static u1 volatile state;
  static void (*stateChangeReceiver)();

public:
  static inline void checkH1() {
    constexpr u1 pos = 2;

    u1 oldState = state;
    u1 newState = oldState;

    if (H1::isActive())
      newState |= 1 << pos;
    else
      newState &= ~(1 << pos);
    state = newState;

    if (newState != oldState && stateChangeReceiver) {
      stateChangeReceiver();
    }
  }

  static inline void checkH2() {
    constexpr u1 pos = 1;

    u1 oldState = state;
    u1 newState = oldState;

    if (H2::isActive())
      newState |= 1 << pos;
    else
      newState &= ~(1 << pos);
    state = newState;

    if (newState != oldState && stateChangeReceiver) {
      stateChangeReceiver();
    }
  }

  static inline void checkH3() {
    constexpr u1 pos = 0;

    u1 oldState = state;
    u1 newState = oldState;

    if (H3::isActive())
      newState |= 1 << pos;
    else
      newState &= ~(1 << pos);
    state = newState;

    if (newState != oldState && stateChangeReceiver) {
      stateChangeReceiver();
    }
  }

  // #ifdef QUANTUM_DRIVE
  // friend void ::INT0_vect();
  // friend void ::INT1_vect();
  // friend void ::PCINT0_vect();
  // #endif
  //
  // #ifdef BED_CONTROLLER
  // friend void ::INT6_vect();
  // friend void ::PCINT0_vect();
  // #endif

public:
  static void init();

  static inline void setStateChangeReceiver(void (*receiver)()) { stateChangeReceiver = receiver; }

  inline static u1 getState() { return state; }

private:
};

}; // namespace ThreePhaseControllerNamespace

#endif /* HALLWATCHER_H */
