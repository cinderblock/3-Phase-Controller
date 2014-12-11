/* 
 * File:   Board.cpp
 * Author: bztacklind
 * 
 * Created on March 25, 2014, 12:46 PM
 */

#include "Board.h"

//From tool collection winAVR
#include <avr/io.h>

//LED - indicator
IOpin Board::LED        = IOpin(&PORTD, 4);

//I2C
IOpin Board::I2C::SCL        = IOpin(&PORTD, 0);
IOpin Board::I2C::SDA        = IOpin(&PORTD, 1);

//SPI
IOpin Board::SPI::SCLK       = IOpin(&PORTB, 1);
IOpin Board::SPI::MOSI       = IOpin(&PORTB, 2);
IOpin Board::SPI::MISO       = IOpin(&PORTB, 3);
IOpin Board::MagSel          = IOpin(&PORTD, 5);

//Input Voltage Checks
IOpin Board::AIN0       = IOpin(&PORTE, 6);
IOpin Board::VBATS      = IOpin(&PORTF, 5);

//Fet controllers
IOpin Board::DRV::AH         = IOpin(&PORTB, 7);
IOpin Board::DRV::AL         = IOpin(&PORTB, 0);
IOpin Board::DRV::BH         = IOpin(&PORTB, 6);
IOpin Board::DRV::BL         = IOpin(&PORTF, 0);
IOpin Board::DRV::CH         = IOpin(&PORTB, 5);
IOpin Board::DRV::CL         = IOpin(&PORTB, 4);
//Fet Voltage Dividers
IOpin Board::SEN::AS         = IOpin(&PORTD, 6);
IOpin Board::SEN::BS         = IOpin(&PORTF, 4);
IOpin Board::SEN::CS         = IOpin(&PORTD, 7);

//Serial
IOpin Board::SER::Rx         = IOpin(&PORTD, 2);
IOpin Board::SER::Tx         = IOpin(&PORTD, 3);

/*
IOpin Board::SCK  = IOpin(&PORTB, 5);
IOpin Board::MOSI = IOpin(&PORTB, 3);
IOpin Board::MISO = IOpin(&PORTB, 4);
IOpin Board::SS   = IOpin(&PORTB, 2);
IOpin Board::Interrupt = IOpin(&PORTB, 1); //arduino mini pro D9 
IOpin Board::LED1 = IOpin(&PORTD, 2);
IOpin Board::LED2 = IOpin(&PORTD, 3);
IOpin Board::PWM1 = IOpin(&PORTD, 4);
//IOpin Board::led3 = IOpin(&PORTE, 0);
//IOpin Board::led2 = IOpin(&PORTE, 1);

// harness soldered into these header pins
// f for front of FileBot
IOpin Board::fButton = IOpin(&PORTD, 2); //arduino mini pro D2
IOpin Board::fSpeed  = IOpin(&PORTD, 3); //arduino mini pro D3
IOpin Board::fSteer  = IOpin(&PORTD, 4); //arduino mini pro D4

// r for rear of FileBot
IOpin Board::rSteer  = IOpin(&PORTD, 5); //arduino mini pro D5
IOpin Board::rSpeed  = IOpin(&PORTD, 6); //arduino mini pro D6
IOpin Board::rButton = IOpin(&PORTD, 7); //arduino mini pro D7

                                         //arduino mini pro D8 PORTB0 is available

//IOpin Board::rEncoder = IOpin(&PORTDC, 0); //arduino mini pro A0\
 */
