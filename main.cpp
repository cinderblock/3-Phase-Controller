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
//#include "Debug.h"
#include "TwillBotInterface.h"
#include "MotorControl.h"
#include "Debug.h"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

void init() {
 TwillBotInterface::init();
 Timer::init();
 MotorControl::init();
 Debug::init();
 Debug::reportByte(0xff);
 // Make sure the ^SS pin is a driven output BEFORE initializing the SPI hardware.
 // The AVR's ^SS pin is really Board::DRV::AL, controlled by ThreePhaseDriver.
 ThreePhaseDriver::init();
 sei();
 MLX90363::init();
}

/*
 * 
 */
void main() {
 
 Board::LED.output();
 Board::LED.off();
 Board::SEN::AS.input();
 Board::SEN::BS.input();
 Board::SEN::CS.input();
// ThreePhaseDriver::debugON();
// 
// 
// ThreePhaseDriver::debugOFF();
 
 //why is this here?
 _delay_ms(100);
 //Board::DRV::AL.off();
 
 sei();
 
 ThreePhaseDriver::setAmplitude(50);
 MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
 
 MLX90363::startTransmitting();
 while (MLX90363::isTransmitting());
 u2 lastTimeSPICheck = Timer::getCurTime();
 const u2 timeBetweenSPIChecks = Timer::lengthUS(1000);
 
 ThreePhaseDriver::advanceTo(100);
 MotorControl::setInitialPosition(100);
 
 TwillBotInterface::reserveNextReadBuffer();
 
 Board::LED.on();
 
 u2 i = 0;
 
 while (1) {
  ThreePhaseDriver::advanceTo(i++);
   _delay_ms(2);
   if (i > 270) i = 0;
 }
 
// MotorControl::goAt(10);

 while(1) {
    
    //Do motor stuff
    MotorControl::advance();
    
    if (Timer::getSince(lastTimeSPICheck) > timeBetweenSPIChecks){
        MLX90363::startTransmitting();
        while (MLX90363::isTransmitting());
        lastTimeSPICheck = Timer::getCurTime();

        TwillBotInterface::releaseNextWriteBuffer();
        u2 * const buff = (u2 * const)TwillBotInterface::getOutgoingWriteBuffer();

        buff[0] = MLX90363::getAlpha();
        buff[1] = MotorControl::getTimer();
        buff[2] = MLX90363::getRoll();
        buff[3] = MLX90363::getErr();
        buff[4] = MotorControl::getStep();
        
    }
    
    if (TwillBotInterface::hasReceivedBlock()){
        TwillBotInterface::reserveNextReadBuffer();
        if (*TwillBotInterface::getIncomingReadBuffer() == 0x33){
            MotorControl::goDistance(1);
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x20){
            MotorControl::goAt(10);
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x21){
            MotorControl::goAt(TwillBotInterface::getIncomingReadBuffer()[1]);
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x35){
            MotorControl::goDistance(-1);
        }
    }
 }
 
 while(1);
}

