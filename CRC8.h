/* 
 * File:   CRC8.h
 * Author: Cameron
 *
 * Created on January 12, 2016, 9:51 PM
 */

#ifndef CRC8_H
#define	CRC8_H

#include <AVR++/basicTypes.h>

#include <avr/pgmspace.h>

using namespace AVR;

class CRC8 {
 u1 crc;
 static const u1 table[256] PROGMEM;
public:

 inline void reset() {
  crc = 0;
 }

 inline CRC8() {
  reset();
 }

 inline void feed(u1 b) {
  crc = pgm_read_byte(&table[crc ^ b]);
 }

 inline u1 getCRC() {
  return crc;
 }

 inline CRC8& operator<<(u1 b) {
  feed(b);
  return *this;
 }

};

#endif	/* CRC8_H */

