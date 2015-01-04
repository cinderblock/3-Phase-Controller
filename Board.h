/* 
 * File:   Board.h
 * Author: bztacklind
 *
 * Created on March 25, 2014, 12:46 PM
 */

#ifndef BOARD_H
#define	BOARD_H

#include "IOpin.h"
#include "Config.h"

//using namespace AVR;

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
};

#endif	/* BOARD_H */