/* 
 * File:   MotorControl.cpp
 * Author: btacklind
 * 
 * Created on March 28, 2015, 4:30 PM
 */

#include "MotorControl.h"

void MotorControl::init(){
 //ThreePhaseDriver::init();
 
 //Enable 8-bit Timer 0 for timing purposes. TOO SMALL OF VALUES
 /*
  * COM0A   COM0A   COM0B   COM0B   ~~~~    ~~~~    WGM0    WGM0
  * 0       0       0       0                       0       0
  * Normal port   | Normal port                   | Normal Mode
  * operations    | operations                    |
  */
 //TCCR0A = 0;
 
 /*
  * FOC0A   FOC0B   ~~~~~   ~~~~~   WGM02    CS02    CS01    CS00
  * 0       0                       0        0       0       1
  * Normal port   |               | Normal  | No pre-scaling
  * operations    |               | Mode    | on CLK
  */
 //TCCR0B = 0b00000101;
 
 /*
  * ~~~~~   ~~~~~   ~~~~~   ~~~~~   ~~~~~  OCIE0B  OCIE0A  TOIE0
  *                                        0       0       0
  *                                          Disabled Interrupts
  */
 //TIMSK0 = 0b00000001;
 
 //16-bit counter
 TCCR3A = 0;
 
 TCCR3B = 0b00000001;

 TCCR3C = 0;
 
 TIMSK3 = 0b00000000;
 
 //count = 0;
}

void MotorControl::go(s1 force) {
    
}

void MotorControl::goAt(s1 speed) {
  
}


void MotorControl::advance(){
    
}

void MotorControl::setInitialPosition(u2 pos){
    lastStep = pos;
    timeLastStep = getTimer();
}