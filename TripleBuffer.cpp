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
void TripleBuffer<size, readInterrupt, writeInterrupt>::reserveNewestBufferForReading() {
 if (readInterrupt) cli();
 switch (state) {
  default: break;
  case State::F: state = State::G; break;
  case State::H: state = State::I; break;
  case State::C:
  case State::J: state = State::E; break;
  case State::D:
  case State::K: state = State::L; break;
  case State::M: state = State::N; break;
  case State::O: state = State::P; break;
 }
 if (readInterrupt) sei();
}

template<u1 size, bool readInterrupt, bool writeInterrupt>
void TripleBuffer<size, readInterrupt, writeInterrupt>::markNewestBuffer() {
 if (writeInterrupt) cli();
 switch (state) {
  default: // WTF
  case State::A: state = State::B; break;
  case State::B:
  case State::D: state = State::C; break;
  case State::C: state = State::D; break;

  case State::E:
  case State::O: state = State::F; break;
  case State::F:
  case State::N: state = State::O; break;
  case State::G:
  case State::M: state = State::H; break;
  case State::H:
  case State::L: state = State::M; break;
  case State::I:
  case State::K: state = State::J; break;
  case State::J:
  case State::P: state = State::K; break;
 }
 if (writeInterrupt) sei();
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
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getWriteBuffer() {
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
u1* TripleBuffer<size, readInterrupt, writeInterrupt>::getReadBuffer() {
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

