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
 using LED0 = Output<Ports::F, 4>; // On breakout
 using LED1 = Output<Ports::D, 5>; // On Simple Controller
 using VBATS = Input<Ports::D, 4, false>;

 using Sw1 = Input<Ports::F, 1>; // start switch pin


 using MagSel = Output<Ports::B, 0, true>;

 using H1 = Input<Ports::E, 6, false, true>;
 using H2 = Input<Ports::B, 7, false, true>;
 using H3 = Input<Ports::B, 4, false, true>;

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

 namespace I2C {
  using SCL = Input<Ports::D, 0>;
  using SDA = Input<Ports::D, 1>;
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
  using AS = Input<Ports::F, 5, false>;
  using BS = Input<Ports::F, 6, false>;
  using CS = Input<Ports::F, 7, false>;
 };

 namespace MUX {
  constexpr u1 AS = 0b000101; // ADC5
  constexpr u1 BS = 0b000110; // ADC6
  constexpr u1 CS = 0b000111; // ADC7
  constexpr u1 VBATS = 0b100000; // ADC8
 }

 constexpr u4 ClockFrequency = F_CPU;
};

};

#endif	/* BOARD_H */
