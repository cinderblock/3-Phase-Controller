/* 
 * File:   SPI.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:10 PM
 */

#ifndef SPI_H
#define	SPI_H

#include <avr/io.h>
#include "bitTypes.h"

namespace AVR {
 namespace SPI {
  typedef union {
   struct {
    b2 Divider:2;
    b1 ClockPhase:1;
    b1 ClockPolarity:1;
    b1 Master:1;
    b1 Order:1;
    b1 Enable:1;
    b1 InterruptEnable:1;
   };
   u1 byte;
  } CRt;

  typedef union {
   struct {
    b1 DoubleSpeed:1;
    u1:5;
    b1 Collision:1;
    b1 InterruptFlag:1;
   };
   u1 byte;
  } SRt;

  constexpr volatile CRt * const CR = (volatile CRt * const)&SPCR;
  constexpr volatile SRt * const SR = (volatile SRt * const)&SPSR;
  constexpr volatile u1  * const DR =                       &SPDR;

 };
};
#endif	/* SPI_H */

