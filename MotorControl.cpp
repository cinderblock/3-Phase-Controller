/* 
 * File:   MotorControl.cpp
 * Author: chtacklind
 * 
 * Created on March 28, 2015, 4:30 PM
 */

#include "MotorControl.h"

void MotorControl::init(){
 ThreePhaseDriver::init();
 /*
  * COM0A   COM0A   COM0B   COM0B   ~~~~    ~~~~    WGM0    WGM0
  * 0       0       0       0                       0       0
  * Normal port   | Normal port                   | Normal Mode
  * operations    | operations                    |
  */
 TCCR0A = 0;
 
}

void MotorControl::go(s1 speed) {
    
}

