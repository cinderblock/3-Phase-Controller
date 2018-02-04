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

namespace ThreePhaseControllerNamespace {

using namespace AVR;

namespace Board {
 using LED = Output<Ports::B, 7>;

 using MagSel = Output<Ports::B, 0, true>;

 using H1 = Input<Ports::D, 0>; // INT0
 using H2 = Input<Ports::D, 1>; // INT1
 using H3 = Input<Ports::B, 4>; // PCINT4
 
 namespace SPI {
  using SCLK = Output<Ports::B, 1>;
  using MOSI = Output<Ports::B, 2>;
  using MISO = Input<Ports::B, 3>;

  inline void slaveDeselect() {MagSel::off();}
  inline void slaveSelect  () {MagSel::on();}

  /**
   * Check if we're still talking on the SPI bus
   * @return 
   */
  inline bool isSlaveSelected() {
   return MagSel::isOn();
  }
 };
 
 namespace SER {
  using Rx = Input<Ports::D, 2>;
  using Tx = Output<Ports::D, 3>;
 };
 namespace DRV {
  using AH = Output<Ports::C, 7>;
  using AL = Output<Ports::C, 6>;
  using BH = Output<Ports::B, 6>;
  using BL = Output<Ports::B, 5>;
  using CH = Output<Ports::D, 7>;
  using CL = Output<Ports::D, 6>;
 };
 namespace SEN {
  using AS = Input<Ports::D, 4, false>;
  using BS = Input<Ports::F, 4, false>;
  using CS = Input<Ports::F, 1, false>;
  using VBATS = Input<Ports::F, 0, false>;
 };
 namespace MUX {
  constexpr u1 AS = 0b100000; // ADC8
  constexpr u1 BS = 4; // ADC4
  constexpr u1 CS = 1; // ADC1
  constexpr u1 VBATS = 0; // ADC0
 }
 
 constexpr u4 ClockFrequency = F_CPU;
};

};

#endif	/* BOARD_H */
