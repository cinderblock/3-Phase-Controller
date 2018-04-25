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

#ifdef BED_CONTROLLER
 using Sw1 = Input<Ports::F, 1>; // start switch pin
 using LED0 = Output<Ports::F, 4>; // On breakout
 using LED1 = Output<Ports::D, 5>; // On Simple Controller
 using LED = LED0;
#endif

#ifdef QUANTUM_DRIVE
 using LED = Output<Ports::B, 7>;
#endif

 using MagSel = Output<Ports::B, 0, true>;

#ifdef QUANTUM_DRIVE
 using H1 = Input<Ports::D, 0>; // INT0
 using H2 = Input<Ports::D, 1>; // INT1
 using H3 = Input<Ports::B, 4>; // PCINT4
#endif

#ifdef BED_CONTROLLER
 using H1 = Input<Ports::E, 6, false, true>; // INT6
 using H2 = Input<Ports::B, 7, false, true>; // PCINT7
 using H3 = Input<Ports::B, 4, false, true>; // PCINT4
#endif
 
 namespace SPI {
  using SCLK = Output<Ports::B, 1>;
  using MOSI = Output<Ports::B, 2>;
  using MISO = Input<Ports::B, 3>;

  inline void slaveDeselect() __attribute__((always_inline, hot));
  inline void slaveSelect() __attribute__((always_inline, hot));
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
 
#ifdef QUANTUM_DRIVE
 namespace SEN {
  using AS = Input<Ports::D, 4, false>; // ADC8
  using BS = Input<Ports::F, 4, false>; // ADC4
  using CS = Input<Ports::F, 1, false>; // ADC1
  using VBATS = Input<Ports::F, 0, false>; // ADC0
 };
 
 namespace MUX {
  constexpr u1 AS = 0b100000; // ADC8
  constexpr u1 BS = 4; // ADC4
  constexpr u1 CS = 1; // ADC1
  constexpr u1 VBATS = 0; // ADC0
 }
#endif

#ifdef BED_CONTROLLER
 namespace SEN {
  using AS = Input<Ports::F, 5, false>; // ADC5
  using BS = Input<Ports::F, 6, false>; // ADC6
  using CS = Input<Ports::F, 7, false>; // ADC7
  using VBATS = Input<Ports::D, 4, false>; // ADC8
 };

 namespace MUX {
  constexpr u1 AS = 0b000101; // ADC5
  constexpr u1 BS = 0b000110; // ADC6
  constexpr u1 CS = 0b000111; // ADC7
  constexpr u1 VBATS = 0b100000; // ADC8
 }
#endif
 
 constexpr u4 ClockFrequency = F_CPU;
};

};

#endif	/* BOARD_H */
