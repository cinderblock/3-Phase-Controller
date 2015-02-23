/* 
 * File:   TripleBuffer.cpp
 * Author: Cameron
 * 
 * Created on February 22, 2015, 5:58 PM
 */

#include <avr/interrupt.h>

#include "TripleBuffer.h"

template <u1 size, bool readInterrupt, bool writeInterrupt>
TripleBuffer<size, readInterrupt, writeInterrupt>::TripleBuffer() :state(State::A) {
}

template<u1 size, bool readInterrupt, bool writeInterrupt>
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getReadBuffer() {
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

template<u1 size, bool readInterrupt, bool writeInterrupt>
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getWriteBuffer() {
 if (writeInterrupt) cli();
 switch (state) {
  default: // WTF
  case State::A: state = State::B; if (writeInterrupt) sei(); return rBuffer;
  case State::B:
  case State::D: state = State::C; if (writeInterrupt) sei(); return gBuffer;
  case State::C: state = State::D; if (writeInterrupt) sei(); return rBuffer;

  case State::E:
  case State::O: state = State::F; if (writeInterrupt) sei(); return bBuffer;
  case State::F:
  case State::N: state = State::O; if (writeInterrupt) sei(); return gBuffer;
  case State::G:
  case State::M: state = State::H; if (writeInterrupt) sei(); return rBuffer;
  case State::H:
  case State::L: state = State::M; if (writeInterrupt) sei(); return bBuffer;
  case State::I:
  case State::K: state = State::J; if (writeInterrupt) sei(); return gBuffer;
  case State::J:
  case State::P: state = State::K; if (writeInterrupt) sei(); return rBuffer;
 }
 // WTF
 return nullptr;
}

template<u1 size, bool readInterrupt, bool writeInterrupt>
bool TripleBuffer<size, readInterrupt, writeInterrupt>::isNewData() {
 // We don't need to lock anything here since we're only reading
 // And because of the machine's states, even if the state changes before a read
 // (and we just returned true), it will stay true until a read
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

template<u1 size, bool readInterrupt, bool writeInterrupt>
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getCurrentWriteBuffer() {
 switch (state) {
  case State::A: return nullptr;
  case State::B: 
  case State::D: 
  case State::H: 
  case State::I: 
  case State::K: 
  case State::L: return rBuffer;
  case State::C: 
  case State::E: 
  case State::J: 
  case State::O: 
  case State::P: return gBuffer;
  case State::F: 
  case State::G: 
  case State::M: 
  case State::N: return bBuffer;
 }
 // WTF
 return nullptr;
}

template<u1 size, bool readInterrupt, bool writeInterrupt>
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getCurrentReadBuffer() {
 switch (state) {
  case State::A: 
  case State::B: 
  case State::C: 
  case State::D: return nullptr;
  case State::E: 
  case State::F: 
  case State::N: 
  case State::O: return rBuffer;
  case State::G: 
  case State::H: 
  case State::L: 
  case State::M: return gBuffer;
  case State::I: 
  case State::J: 
  case State::K: 
  case State::P: return bBuffer;
 }
 // WTF
 return nullptr;
}

