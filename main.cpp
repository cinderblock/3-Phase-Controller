/* 
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include <util/delay.h>
#include <avr/wdt.h>
#include "Board.h"

#include "ThreePhaseController.h"
#include "MLX90363.h"
#include "TwillBotInterface.h"
#include "MotorControl.h"
#include "Debug.h"
#include "Clock.h"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

void init() {
 wdt_reset();
 wdt_disable();
 Debug::init();

 sei();
 
 ::Clock::init();
 
 TwillBotInterface::init();
 
 ThreePhaseController::init();
 
 MotorControl::init();
 
 // End of init
 Board::LED.off();
}

/*
 * 
 */
void main() {
 
 u2 pos = 0;
 
 Clock::MicroTime t(0);
 Clock::MicroTime delta(1);
 Clock::MicroTime now;
 
 while (0) {
  PINE |= 1<<6;
  _delay_us(10);
 }
 
 while (0) {
 }
 
 u1 print = 0;
 
 s2 torque = 20;
 
 ThreePhaseController::setTorque(torque);
 
 auto lastV = ThreePhaseController::getVelocity();
 
 while (1) {
  ThreePhaseController::updateDriver();
  
  continue;
  
  auto v = ThreePhaseController::getVelocity();
  if (v == lastV) continue;
  lastV = v;
  
  if (v > 20) torque--; else if (v < 15) torque++;
  if (torque >  20) torque =  20;
  if (torque < -20) torque = -20;
  ThreePhaseController::setTorque(torque);
 }
 
 ThreePhaseDriver::setAmplitude(10);
 ThreePhaseDriver::advanceTo(0);
 
 while (1);
 
 while (1) {
  t += delta;
  do Clock::readTime(now);
  while (t > now);
  
  if (!print) {
//   Debug::reportClock();
   Debug::reportU2(pos);
  }
  
  ThreePhaseDriver::advanceTo(pos);
  
  if (!print) {
   Debug::reportU2(MLX90363::getAlpha());
//   Debug::reportHexByte(MLX90363::getRoll());
   Debug::endLine();
  }
  
  if (MLX90363::isMeasurementReady())
   MLX90363::startTransmitting();
  
  pos -= 1;
  if (pos >= 0x300) pos += 0x300;
  
  print++;
  if (print >= 5) print = 0;
 }

 
 TwillBotInterface::reserveNextReadBuffer();

 while(1) {
    
    //Do motor stuff
    MotorControl::advance();
    
    if (false /*Timer::getSince(lastTimeSPICheck) > timeBetweenSPIChecks*/){
        MLX90363::startTransmitting();
        while (MLX90363::isTransmitting());
//        lastTimeSPICheck = Timer::getCurTime();

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

