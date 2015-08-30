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
 MotorControl::init();
 Debug::init();
}

/*
 * 
 */
void main() {
 
 Board::LED.output();
 Board::LED.off();
 
 _delay_ms(100);
 
 sei();
 
 //Board::SEN::BS.on();
 //Board::SEN::BS.output();
 //Board::SEN::BS.off();
 
 ThreePhaseDriver::setAmplitude(15);
 MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
 u2 step = 0;
 
 MLX90363::startTransmitting();
 while (MLX90363::isTransmitting());
 
// MotorControl::setInitialPosition();
 
// while(*TwillBotInterface::getIncomingReadBuffer() != 0x33){
//     TwillBotInterface::reserveNextReadBuffer();
// }
 Board::LED.on();

 //u1 N = 0x20;
 //b1 forward = 1;
 
 while(1) {
    
     //SPI doesn't work without this delay
     //Most likely due to the Slave Select not going Low for long enough
    _delay_ms(1);
    
    //ThreePhaseDriver::advance();
    //ThreePhaseDriver::advanceTo(step);
     
    TwillBotInterface::releaseNextWriteBuffer();
    u2 * const buff = (u2 * const)TwillBotInterface::getOutgoingWriteBuffer();
    
    buff[0] = MLX90363::getAlpha();
    buff[1] = MotorControl::getTimer();
    buff[2] = MLX90363::getRoll();
    buff[3] = MLX90363::getErr();
    buff[4] = step==0x300 ? step = 0 : ++step;
    
    MotorControl::advance();
    
    MLX90363::startTransmitting();
    while (MLX90363::isTransmitting());
    
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

