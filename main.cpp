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
#include "Debug.h"
#include "TwillBotInterface.h"
#include "MotorControl.h"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

void init() {
 // Make sure the ^SS pin is a driven output BEFORE initializing the SPI hardware.
 // The AVR's ^SS pin is really Board::DRV::AL, controlled by ThreePhaseDriver.
 ThreePhaseDriver::init();
 MLX90363::init();
 TwillBotInterface::init();
 Debug::init();
}

/*
 * 
 */
void main() {
 
 Board::LED.output();
 Board::LED.on();
 
 _delay_ms(100);
 
 sei();
 
 //Board::SEN::BS.on();
 //Board::SEN::BS.output();
 //Board::SEN::BS.off();
 
 ThreePhaseDriver::setAmplitude(15);
 MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
 u2 step = 0;
 
// while(*TwillBotInterface::getIncomingReadBuffer() != 0x33){
//     TwillBotInterface::reserveNextReadBuffer();
// }

 Board::LED.off();
 //u1 N = 0x20;
 b1 forward = 1;
 
 while(1) {
    TwillBotInterface::releaseNextWriteBuffer();
    
    _delay_ms(100);
    
    //ThreePhaseDriver::advance();
    //ThreePhaseDriver::advanceTo(step);
     
    MLX90363::startTransmitting();
    while (MLX90363::isTransmitting());
    
    u2 * const buff = (u2 * const)TwillBotInterface::getOutgoingWriteBuffer();
    
    buff[0] = MLX90363::getAlpha();
    buff[1] = MLX90363::getBeta();
    buff[2] = MLX90363::getRoll();
    buff[3] = MLX90363::getErr();
    buff[4] = ++step;
    
    
    //Debug::reportByte(step >> 2);
//    if (forward) {
//        if (++step == 0x300)
//            forward = 0;
//    }
//    else{
//        if (--step == 0)
//            forward = 1;
//    }
    //TwillBotInterface::releaseNextWriteBuffer();
    //for(u1 i = 0; i<10;i++)
       //TwillBotInterface::getOutgoingWriteBuffer()[i] = (u1)(N+i);
  

    //N+=0x01;
 }
// while (1) {
//  _delay_us(100);
//  ThreePhaseDriver::advance();
// }
 /*
 do {
  MLX90363::prepareGET1Message(MLX90363::Marker::XYZ);
  
  while (MLX90363::isTransmitting());
 } while (MLX90363::handleResponse());
 */
// Debug::LED.on();

 while(1);
}

