/* 
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include <util/delay.h>
#include "Board.h"

#include "SPI.h"
#include "ThreePhaseDriver.h"
#include "MLX90363.h"

/*
 * 
 */
void main() {
 
 
// Board::LED.output();
 Board::LED.on();
 
 /** Make sure the ^SS pin is a driven output BEFORE initializing the SPI hardware.
  */
 ThreePhaseDriver::init();
 MLX90363::init();

 while(1);
}

