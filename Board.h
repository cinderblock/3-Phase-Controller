/* 
 * File:   Board.h
 * Author: bztacklind
 *
 * Created on March 25, 2014, 12:46 PM
 */

#ifndef BOARD_H
#define	BOARD_H

#include <AVR++/IOpin.h>
#include "Config.h"
#include <avr/io.h>

using namespace AVR;

namespace Board {
 extern IOpin LED;
 extern IOpin VBATS;
 extern IOpin AIN0;
 extern IOpin MagSel;
 namespace I2C {
  extern IOpin SCL;
  extern IOpin SDA;
 };
 namespace SPI {
  extern IOpin SCLK;
  extern IOpin MOSI;
  extern IOpin MISO;

  inline void slaveDeselect() {PORTD |=  (1<<2);}
  inline void slaveSelect  () {PORTD &= ~(1<<2);}

  /**
   * Check if we're still talking on the SPI bus
   * @return 
   */
  inline bool isSlaveSelected() {
   //return !SS.isHigh();
   return !(PORTD & (1<<2));
  }
 };
 namespace SER {
  extern IOpin Rx;
  extern IOpin Tx;
 };
 namespace DRV {
  extern IOpin AH;
  extern IOpin BH;
  extern IOpin CH;
  extern IOpin AL;
  extern IOpin BL;
  extern IOpin CL;
 };
 namespace SEN {
  extern IOpin AS;
  extern IOpin BS;
  extern IOpin CS;
 };
 namespace MUX {
  extern u1 AS;
  extern u1 BS;
  extern u1 CS;
  extern u1 VBATS;
 }
 
 constexpr u4 ClockSpeed = F_CPU;
};

#endif	/* BOARD_H */