/* 
 * File:   Debug.h
 * Author: Cameron
 *
 * Created on January 9, 2015, 4:09 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H

#include "Board.h"


namespace Debug {
 extern IOpin &LED;

 void init();
 
 void endLine();
 
 void sendByte(u1 const);
 void reportU1(u1 const);
 inline void reportU2(u2 const w) {
  reportU1(w);
  reportU1(w >> 8);
 }
 inline void reportU4(u4 const l) {
  reportU2(l);
  reportU2(l >> 16);
 }
 inline void reportS1(s1 const b) {
  reportU1(b);
 }
 inline void reportS2(s2 const w) {
  reportU2(w);
 }
 inline void reportS4(s4 const l) {
  reportU4(l);
 }
 
 void reportClock();

 void sendHeader();
 void sendEnd();
 
 class Printer {
 public:
  enum class Special : u1 {Start, End};
  
  inline Printer& operator<< (const Special s) {
   if (s == Special::Start) {
    Debug::sendHeader();
   } else if (s == Special::End) {
    Debug::sendEnd();
   }
   return *this;
  }
  
  inline Printer& operator<< (const u1 b) {
   Debug::reportU1(b);
   return *this;
  }

  inline Printer& operator<< (const u2 w) {
   Debug::reportU2(w);
   return *this;
  }

  inline Printer& operator<< (const u4 l) {
   Debug::reportU4(l);
   return *this;
  }
  
  inline Printer& operator<< (const s1 b) {
   Debug::reportS1(b);
   return *this;
  }

  inline Printer& operator<< (const s2 w) {
   Debug::reportS2(w);
   return *this;
  }

  inline Printer& operator<< (const s4 l) {
   Debug::reportS4(l);
   return *this;
  }
  
 };
 
 namespace TwillBotInterface {
  constexpr bool LEDs = false;

  namespace ISR {
   inline static void enter() {
    if (!LEDs) return;
    Board::LED.off();
   }

   inline static void exit() {
    if (!LEDs) return;
    Board::LED.on();
   }
  }

	namespace I2C {
		inline static void printStatus() {

		}
		inline static void SlaveNACK() {

		}
		inline static void SlaveStopped() {

		}
		inline static void unhandledStatus() {
			Board::LED.on();
		}
	}

  inline static void timeout() {
   if (!LEDs) return;
   Board::LED.off();
  }
 }
 
 extern Printer SOUT;
};

#endif	/* DEBUG_H */

