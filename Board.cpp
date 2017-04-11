/* 
 * File:   Board.cpp
 * Author: bztacklind
 * 
 * Created on March 25, 2014, 12:46 PM
 */

#include "Board.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

// LED - indicator
IOpin Board::LED        = IOpin(&PORTB, 7);

// I2C
IOpin Board::I2C::SCL        = IOpin(&PORTD, 0);
IOpin Board::I2C::SDA        = IOpin(&PORTD, 1);

// SPI
IOpin Board::SPI::SCLK       = IOpin(&PORTB, 1);
IOpin Board::SPI::MOSI       = IOpin(&PORTB, 2);
IOpin Board::SPI::MISO       = IOpin(&PORTB, 3);
IOpin Board::MagSel          = IOpin(&PORTD, 2);

// Input Voltage Checks
IOpin Board::AIN0       = IOpin(&PORTE, 6);
IOpin Board::VBATS      = IOpin(&PORTF, 5);
u1    Board::MUX::VBATS = 0b000101; // ADC5

// FET control lines
IOpin Board::DRV::AH         = IOpin(&PORTC, 7);
IOpin Board::DRV::AL         = IOpin(&PORTC, 6);
IOpin Board::DRV::BH         = IOpin(&PORTB, 6);
IOpin Board::DRV::BL         = IOpin(&PORTB, 5);
IOpin Board::DRV::CH         = IOpin(&PORTD, 7);
IOpin Board::DRV::CL         = IOpin(&PORTD, 6);

// Back EMF sens pins
IOpin Board::SEN::AS         = IOpin(&PORTB, 4);
IOpin Board::SEN::BS         = IOpin(&PORTF, 4);
IOpin Board::SEN::CS         = IOpin(&PORTD, 4);

// Back EMF sens ADC MUX values
u1    Board::MUX::AS = 0b100011; // ADC11
u1    Board::MUX::BS = 0b000100; // ADC4
u1    Board::MUX::CS = 0b100000; // ADC8

// Serial
//IOpin Board::SER::Rx         = IOpin(&PORTD, 2);
IOpin Board::SER::Tx         = IOpin(&PORTD, 3);
