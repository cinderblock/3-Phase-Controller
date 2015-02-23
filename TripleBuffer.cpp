/* 
 * File:   TripleBuffer.cpp
 * Author: Cameron
 * 
 * Created on February 22, 2015, 5:58 PM
 */

#include <avr/interrupt.h>

#include "TripleBuffer.h"

template <u1 size>
TripleBuffer<size>::TripleBuffer() :state(State::A) {
}

template <u1 size>
TripleBuffer<size>::TripleBuffer(const TripleBuffer& orig) {
}

template <u1 size>
TripleBuffer<size>::~TripleBuffer() {
}

template<u1 size>
u1* TripleBuffer<size>::getReadBuffer() {
 cli();
 switch (state) {
  case State::A:
  case State::B:                   sei(); return nullptr;

  case State::E:                   sei(); return rBuffer;
  case State::F: state = State::G;
  case State::G:                   sei(); return gBuffer;
  case State::H: state = State::I;
  case State::I:                   sei(); return bBuffer;
  case State::C:
  case State::J: state = State::E; sei(); return rBuffer;
  case State::D:
  case State::K: state = State::L;
  case State::L:                   sei(); return gBuffer;
  case State::M: state = State::N;
  case State::N:                   sei(); return rBuffer;
  case State::O: state = State::P;
  case State::P:                   sei(); return bBuffer;
 }
}

template<u1 size>
u1* TripleBuffer<size>::getWriteBuffer() {
 cli();
 switch (state) {
  case State::A: state = State::B; sei(); return rBuffer;
  case State::B:
  case State::D: state = State::C; sei(); return gBuffer;
  case State::C: state = State::D; sei(); return rBuffer;

  case State::E:
  case State::O: state = State::F; sei(); return bBuffer;
  case State::F:
  case State::N: state = State::O; sei(); return gBuffer;
  case State::G:
  case State::M: state = State::H; sei(); return rBuffer;
  case State::H:
  case State::L: state = State::M; sei(); return bBuffer;
  case State::I:
  case State::K: state = State::J; sei(); return gBuffer;
  case State::J:
  case State::P: state = State::K; sei(); return rBuffer;
 }
}

template<u1 size>
bool TripleBuffer<size>::isNewData() {
 switch (state) {
  case State::A:
  case State::B:
  case State::E:
  case State::G:
  case State::I:
  case State::L:
  case State::N:
  case State::P: return false;
  case State::C:
  case State::D:
  case State::F:
  case State::H:
  case State::J:
  case State::K:
  case State::M:
  case State::O: return true;
 }
}
