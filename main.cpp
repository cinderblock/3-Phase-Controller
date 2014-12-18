/* 
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include <util/delay.h>
#include "Board.h"

#include "ThreePhaseDriver.h"
#include "MLX90363.h"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

void init() {
 Board::LED.output();
 Board::LED.off();

 // Make sure the ^SS pin is a driven output BEFORE initializing the SPI hardware.
 // The AVR's ^SS pin is really Board::DRV::AL, controlled by ThreePhaseDriver.
 ThreePhaseDriver::init();
 MLX90363::init();
}

/*
 * 
 */
void main() {
 
 _delay_ms(100);
 
 sei();
 
 u1 num = 0;
 
 do {
  MLX90363::sendGET1Message(MLX90363::Marker::AlphaBeta);
//  num = MLX90363::num;
  
  num =255;
  
  Board::LED.on();
  _delay_us(num);
  Board::LED.off();
  _delay_us(256-num);
  
  while (MLX90363::isTransmitting());
 } while (MLX90363::handleResponse());
 
// Board::LED.on();

 while(1);
}

