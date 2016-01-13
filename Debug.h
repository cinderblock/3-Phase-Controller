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
};

#endif	/* DEBUG_H */

