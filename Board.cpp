/* 
 * File:   Board.cpp
 * Author: bztacklind
 * 
 * Created on March 25, 2014, 12:46 PM
 */

#include "Board.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;
u1    Board::MUX::VBATS = 0b000101; // ADC5

// Back EMF sens ADC MUX values
u1    Board::MUX::AS = 0b100011; // ADC11
u1    Board::MUX::BS = 0b000100; // ADC4
u1    Board::MUX::CS = 0b100000; // ADC8
