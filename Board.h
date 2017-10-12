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
 using LED = IOpin<Ports::B, 7>;
 using VBATS = IOpin<Ports::F, 5>;
 using AIN0 = IOpin<Ports::E, 6>;
 
 using MagSel = IOpin<Ports::D, 2>;

 namespace I2C {
  using SCL = IOpin<Ports::D, 0>;
  using SDA = IOpin<Ports::D, 1>;
 };
 namespace SPI {
  using SCLK = IOpin<Ports::B, 1>;
  using MOSI = IOpin<Ports::B, 2>;
  using MISO = IOpin<Ports::B, 3>;
  using AVRss = IOpin<Ports::B, 0>;

  inline void setupIO() {
    MagSel::output();
    
    // Setup "User Defined" hardware lines
    SCLK::output();
    MOSI::output();

    // Don't forget the AVR's hardware SS line!
    // If the AVR's SS is left as an input and it transitions low, SPI hardware
    // kicked out of master mode.
    AVRss::on();
    AVRss::output();

    // SPI hardware does this for us, but do it anyway
    SCLK::off();
    MISO::input();
    MISO::on();
  }
  inline void slaveDeselect() {MagSel::on();}
  inline void slaveSelect  () {MagSel::off();}

  /**
   * Check if we're still talking on the SPI bus
   * @return 
   */
  inline bool isSlaveSelected() {
   return !MagSel::isHigh();
  }
 };
 namespace SER {
  using Rx = IOpin<Ports::D, 2>;
  using Tx = IOpin<Ports::D, 3>;
 };
 namespace DRV {
  using AH = IOpin<Ports::C, 7>;
  using BH = IOpin<Ports::B, 6>;
  using CH = IOpin<Ports::D, 7>;
  using AL = IOpin<Ports::C, 6>;
  using BL = IOpin<Ports::B, 5>;
  using CL = IOpin<Ports::D, 6>;
 };
 namespace SEN {
  using AS = IOpin<Ports::B, 4>;
  using BS = IOpin<Ports::F, 4>;
  using CS = IOpin<Ports::D, 4>;
 };
 namespace MUX {
  constexpr u1 AS = 0b100011; // ADC11
  constexpr u1 BS = 0b000100; // ADC4
  constexpr u1 CS = 0b100000; // ADC8
  constexpr u1 VBATS = 0b000101; // ADC5
 }
 
 constexpr u4 ClockSpeed = F_CPU;
};

};

#endif	/* BOARD_H */
