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
 //Debug::init();
}

/*
 * 
 */
void main() {
 
 Board::LED.output();
 Board::LED.off();
 
 //why is this here?
 _delay_ms(100);
 
 sei();
 
 ThreePhaseDriver::setAmplitude(30);
 MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
 
 MLX90363::startTransmitting();
 while (MLX90363::isTransmitting());
 u2 lastTimeSPICheck = Timer::getCurTime();
 const u2 timeBetweenSPIChecks = Timer::lengthUS(1000);
 
 u2 lastTimei2cCheck = Timer::getCurTime();
 const u2 timeBetweeni2cChecks = Timer::lengthUS(1000);
 
 
 ThreePhaseDriver::advanceTo(40);
 MotorControl::setInitialPosition(40);
 
 TwillBotInterface::reserveNextReadBuffer();
 
 Board::LED.on();

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
            //*TwillBotInterface::getIncomingReadBuffer() = 0;
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x20){
            MotorControl::goAt(10);
            //*TwillBotInterface::getIncomingReadBuffer() = 0;
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x21){
            MotorControl::goAt(TwillBotInterface::getIncomingReadBuffer()[1]);
            //*TwillBotInterface::getIncomingReadBuffer() = 0;
            //TwillBotInterface::getIncomingReadBuffer()[1] = 0;
        }
        else if (*TwillBotInterface::getIncomingReadBuffer() == 0x35){
            MotorControl::goDistance(-1);
        }
    }
 }
 
 while(1);
}

