/* 
 * File:   Debug.cpp
 * Author: Cameron
 * 
 * Created on January 9, 2015, 4:09 PM
 */

#include "Debug.h"
#include "Clock.h"
#include <avr/io.h>
#include <util/crc16.h>

IOpin& Debug::LED = Board::LED;

static u1 CRC;

void Debug::init() {
 LED.output();
 LED.on();
 
 UBRR1 = 0;
 
 // Set default
 UCSR1D = 0b00;
 
 // Set default
 UCSR1C = 0b00000110;
 
 // Set default
 UCSR1A = 0b00000000;
 
 // Enable transmitter
 UCSR1B = 0b00001000;
}

void Debug::reportByte(const u1 c) {
 while (!(UCSR1A & (1 << UDRE1)));
 UDR1 = c;
}

u1 nibToHex(u1 const nib) {
 if (nib < 10)
  return '0' + nib;
 if (nib < 16)
  return 'A' - 10 + nib;
 return '*';
}

void Debug::reportHexByte(const u1 b) {
 CRC = _crc8_ccitt_update(CRC, b);
 reportByte(nibToHex((b >> 4) & 0xf));
 reportByte(nibToHex((b >> 0) & 0xf));
}

void Debug::reportPhase(const u2 p) {
 CRC = _crc8_ccitt_update(CRC, p >> 8);
 CRC = _crc8_ccitt_update(CRC, p);
 reportByte(nibToHex((p >> 8) & 0xf));
 reportByte(nibToHex((p >> 4) & 0xf));
 reportByte(nibToHex((p >> 0) & 0xf));
}

void Debug::reportMag(const u2 p) {
 CRC = _crc8_ccitt_update(CRC, p >> 8);
 CRC = _crc8_ccitt_update(CRC, p);
 reportByte(nibToHex((p >> 12) & 0xf));
 reportByte(nibToHex((p >> 8) & 0xf));
 reportByte(nibToHex((p >> 4) & 0xf));
 reportByte(nibToHex((p >> 0) & 0xf));
}

void Debug::reportClock() {
 u4 t;
 Clock::readTime(t);
 reportMag(t);
}

void Debug::endLine() {
 reportHexByte(CRC);
 CRC = 0xff;
 reportByte('\r');
 reportByte('\n');
}