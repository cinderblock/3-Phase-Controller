/* 
 * File:   TripleBuffer.cpp
 * Author: Cameron
 * 
 * Created on February 22, 2015, 5:58 PM
 */

#include <avr/interrupt.h>

#include "TripleBuffer.h"

template <u1 size, bool readInterrupt>
TripleBuffer<size, readInterrupt>::TripleBuffer() :state(State::A) {
}

template<u1 size, bool readInterrupt>
u1* TripleBuffer<size, readInterrupt>::getReadBuffer() {
 if (readInterrupt) cli();
 switch (state) {
  default: // WTF
  case State::A:
  case State::B:                   if (readInterrupt) sei(); return nullptr;

  case State::E:                   if (readInterrupt) sei(); return rBuffer;
  case State::F: state = State::G;
  case State::G:                   if (readInterrupt) sei(); return gBuffer;
  case State::H: state = State::I;
  case State::I:                   if (readInterrupt) sei(); return bBuffer;
  case State::C:
  case State::J: state = State::E; if (readInterrupt) sei(); return rBuffer;
  case State::D:
  case State::K: state = State::L;
  case State::L:                   if (readInterrupt) sei(); return gBuffer;
  case State::M: state = State::N;
  case State::N:                   if (readInterrupt) sei(); return rBuffer;
  case State::O: state = State::P;
  case State::P:                   if (readInterrupt) sei(); return bBuffer;
 }
 // WTF
 return nullptr;
}

template<u1 size, bool readInterrupt>
u1* TripleBuffer<size, readInterrupt>::getWriteBuffer() {
 cli();
 switch (state) {
  default: // WTF
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
 // WTF
 return nullptr;
}

template<u1 size, bool readInterrupt>
bool TripleBuffer<size, readInterrupt>::isNewData() {
 switch (state) {
  case State::C:
  case State::D:
  case State::F:
  case State::H:
  case State::J:
  case State::K:
  case State::M:
  case State::O: return true;
  // Prevents a warning
  default: break;
 };
 return false;
}
