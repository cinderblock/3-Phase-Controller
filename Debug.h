/* 
 * File:   Debug.h
 * Author: Cameron
 *
 * Created on January 9, 2015, 4:09 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H

#include "Board.h"
#include "DecPrintFormatter.h"

namespace ThreePhaseControllerNamespace {

namespace Debug {
 using LED = Board::LED;

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

 void start();
 void end();
 void marker();
 
 class Printer {
 public:
  enum class Special : u1 {Start, End, Marker};
  
  inline Printer& operator<< (const Special s) {
   if (s == Special::Start) {
    Debug::start();
   } else if (s == Special::End) {
    Debug::end();
   } else if (s == Special::Marker) {
     Debug::marker();
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
    LED::on();
   }

   inline static void exit() {
    if (!LEDs) return;
    LED::off();
   }
  }

	namespace I2C {
		inline static void printStatus() {

		}
		inline static void SlaveNACK() {

		}
		inline static void SlaveStopped() {

		}
		inline static void unhandledStatus(u1 s = 0) {
			LED::on();
		}
	}

  inline static void timeout() {
   if (!LEDs) return;
   LED::off();
  }
 }
 
 extern Printer SOUT;
 extern libCameron::DecPrintFormatter dout;
};

};

#endif	/* DEBUG_H */
