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
 Clock::MicroTime delta = 25_ms;
 Clock::MicroTime now;
 
 u1 print = 0;
 
 s2 torque = 0; 
 
 u1 const maxTorque = 40;
 
 s1 step = 1;
 
 ThreePhaseController::setTorque(torque);
 
 auto lastV = ThreePhaseController::getVelocity();
 
 while(1){
  ThreePhaseController::updateDriver();

  if(TwillBotInterface::hasReceivedBlock()){
    TwillBotInterface::reserveNextReadBuffer();
    
    u1* incoming = TwillBotInterface::getIncomingReadBuffer();
    if(incoming[0] == 0x20){
      torque = *((s2*)(incoming+1));
      
      if(torque > maxTorque)
        torque = maxTorque;
      else if ( torque < -maxTorque)
        torque = -maxTorque;

      ThreePhaseController::setTorque(torque);
    }
  }

 }

 while(1);
}

