/* 
 * File:   MotorControl.cpp
 * Author: btacklind
 * 
 * Created on March 28, 2015, 4:30 PM
 */

#include "MotorControl.h"
#include "ThreePhaseDriver.h"

void MotorControl::init(){
    
 //16-bit counter Enables
 /*
  * COM3A1  COM3A0  COM3B1  COM3B0  COM3C1  COM3C0    WGM31   WGM30
  * 0       0       0       0       0       0         0       0
  * Normal port   | Normal port     | Normal port    | Normal Mode
  * operations    | operations      | operations     |
  */
 TCCR3A = 0b00000000;
 
 /*
  * ICNC3    ICES3   ~~~~~    WGM33    WGM32    CS32    CS31   CS30
  * 0        0                0        0        1       0      0
  * Noise   |Edge            | Normal          | 1/256 pre-scaling
  * Canceler|Select          | Mode            | on CLK (~1 cycle/sec)
  */
 TCCR3B = 0b00000100;
 
 /*
  * FOC3A    ~~~~~   ~~~~~    ~~~~~    ~~~~~    ~~~~~   ~~~~~   ~~~~~  
  * 0
  * Force Output 
  * Compare (disable)
  */
 TCCR3C = 0b00000000;
 
 /*
  * ~~~~~   ~~~~~   ICIE3   ~~~~~   OCIE3C  OCIE3B  OCIE0A  TOIE0 
  *                 0               0       0       0       0
  *                Input Capture   | Disabled Interrupts
  *               Interrupt Enable |
  */
 TIMSK3 = 0b00000000;
 
 
}

u2 MotorControl::lastStep;
u2 MotorControl::timeLastStep;

void MotorControl::setInitialPosition(u4 pos){
    
    lastStep = getStepFromLocation(pos);
    timeLastStep = getTimer();
}

u2 MotorControl::getStepFromLocation(u4 magData){
    return magData;
}

void MotorControl::go(s1 force) {
    
}

void MotorControl::goAt(s2 speed) {
    
}


void MotorControl::advance(){
    u2 currTime = getTimer();
    u2 timeDiff;
    if (currTime > timeLastStep)
        timeDiff = currTime - timeLastStep;
    else
        timeDiff = currTime + (0xFFFF - timeLastStep);
    
    s2 stepSize = -(timeDiff / 40);
    
    if (stepSize != 0){
        s2 nextStep = lastStep + stepSize;
    
        if (nextStep > 0x2FF) nextStep -= 0x2FF;
        else if (nextStep < 0) nextStep += 0x2FF;
    
        timeLastStep = currTime;
        lastStep = nextStep;
        ThreePhaseDriver::advanceTo(nextStep);
    }
}



