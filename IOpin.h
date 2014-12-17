/*
* File: IOpin.h
* Author: chtacklind
*
* Created on May 5, 2010, 3:23 PM
*/

#ifndef _IOPIN_H
#define _IOPIN_H

#include <avr/sfr_defs.h>
#include "bitTypes.h"

#define PORT(port) _MMIO_BYTE(port - 0)
#define DDR(port) _MMIO_BYTE(port - 1)
#define PIN(port) _MMIO_BYTE(port - 2)

class IOpin {
 
volatile u1 * const port;
u1 const mask;
public:
IOpin(volatile u1 * const port, b3 const pin): port(port), mask(_BV(pin)) {}

/* Sets bit in DDRx */
inline void output() {DDR(port) |= mask;}
/* Clears bit in DDRx */
inline void input() {DDR(port) &= ~mask;}



/* Sets bit in PORTx */
inline void on() {PORT(port) |= mask;}

/* Clears bit in PORTx */
inline void off() {PORT(port) &= ~mask;}

/* Sets bit in PINx */
inline void tgl() {PIN(port) = mask;}

/* Returns value of bit in PINx. Not 0 or 1 but 0 or _BV(pin) */
inline bool isHigh() {return PIN(port) & mask;}

/* Turns on() or off() */
inline void set(bool v) {if (v) on(); else off();}
inline bool operator= (bool v) {set(v); return v;}

inline void operator! ( ) {tgl();}
inline void operator++ (int) {tgl();}
};

#undef PORT
#undef DDR
#undef PIN

#endif /* _IOPIN_H */
