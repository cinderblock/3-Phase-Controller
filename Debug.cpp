/* 
 * File:   Debug.cpp
 * Author: Cameron
 * 
 * Created on January 9, 2015, 4:09 PM
 */

#include "Debug.h"
#include <avr/io.h>

IOpin & Debug::LED = Board::LED;

void Debug::init() {
 LED.output();
 LED.on();
 
 UBRR1 = 0;
 UCSR1D = 0b00;
 UCSR1C = 0b00000110;
 UCSR1A = 0b00000000;
 UCSR1B = 0b00001000;
}

void Debug::reportByte(const u1 c) {
 UDR1 = c;
}