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

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

void init() {
 Board::LED.output();

 // Make sure the ^SS pin is a driven output BEFORE initializing the SPI hardware.
 // The AVR's ^SS pin is really Board::DRV::AL, controlled by ThreePhaseDriver.
 ThreePhaseDriver::init();
 MLX90363::init();
}

/*
 * 
 */
void main() {
 Board::LED.on();

 while(1);
}

