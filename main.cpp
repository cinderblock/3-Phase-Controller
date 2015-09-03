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
 Timer::init();
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
 
 ThreePhaseDriver::setAmplitude(30);
 MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
 //u2 step = 0;
 
 MLX90363::startTransmitting();
 while (MLX90363::isTransmitting());
 u2 lastCheckedTime = Timer::getCurTime();
 const u2 timeBetweenChecks = Timer::lengthUS(1000);
 
 
 MotorControl::setInitialPosition(0);
 
 Board::LED.on();

 //u1 N = 0x20;
 //b1 forward = 1;
 
 
 
 while(1) {
    
    //SPI doesn't work without this delay
    //Most likely due to the Slave Select not going Low for long enough
    //_delay_ms(1);
    
    //Do motor stuff
    MotorControl::advance();
    //ThreePhaseDriver::advance();
    //ThreePhaseDriver::advanceTo(step);
    
    if (Timer::getSince(lastCheckedTime) > timeBetweenChecks){
        MLX90363::startTransmitting();
        while (MLX90363::isTransmitting());
        lastCheckedTime = Timer::getCurTime();

        TwillBotInterface::releaseNextWriteBuffer();
        u2 * const buff = (u2 * const)TwillBotInterface::getOutgoingWriteBuffer();

        buff[0] = MLX90363::getAlpha();
        buff[1] = MotorControl::getTimer();
        buff[2] = MLX90363::getRoll();
        buff[3] = MLX90363::getErr();
        buff[4] = MotorControl::getStep();
        
    }
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

