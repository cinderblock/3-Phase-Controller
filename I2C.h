/* 
 * File:   I2C.h
 * Author: Cameron
 *
 * Created on January 12, 2015, 12:15 AM
 */

#ifndef I2C_H
#define	I2C_H

#include <avr/io.h>
#include "bitTypes.h"

namespace AVR {
 namespace I2C {
  
  typedef union {
   struct {
    b1 InterruptEnable:1;
    u1:1;
    b1 Enable:1;
    b1 WriteCollisionFlag:1;
    b1 StopCondition:1;
    b1 StartCondition:1;
    b1 EnableAcknowledge:1;
    b1 InterruptFlag:1;
   };
   u1 byte;
  } CRt;

  typedef union {
   struct {
    b2 Prescaler:2;
    u1:1;
    b5 Status:5;
   };
   u1 byte;
  } SRt;

  typedef union {
   struct {
    b1 GeneralCallRecognition:1;
    b7 SlaveAddress:7;
   };
   u1 byte;
  } ARt;

  constexpr volatile u1  * const BR  =                       &TWBR;
  constexpr volatile CRt * const CR  = (volatile CRt * const)&TWCR;
  constexpr volatile SRt * const SR  = (volatile SRt * const)&TWSR;
  constexpr volatile u1  * const DR  =                       &TWDR;
  constexpr volatile ARt * const AR  = (volatile ARt * const)&TWAR;
  constexpr volatile u1  * const AMR =                       &TWAMR;

 };
};

#endif	/* I2C_H */

