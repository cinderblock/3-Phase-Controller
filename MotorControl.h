/* 
 * File:   MotorControl.h
 * Author: chtacklind
 *
 * Created on March 28, 2015, 4:30 PM
 */

#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H
#include "ThreePhaseDriver.h"

/*
 * Uses 8-bit timer 0 to have a consistent time base for motor driving
 * 
 */

class MotorControl {
public:
    static void init();
    static void go(u1 force);
    static void goAt(u1 speed);
    static void goTo(u2 position);
    
private:
    
    //static u1 stepSize = 1;
    /*
     * 0: forward
     * 1: backward
     */
    //static b1 dir = 0; 

};

#endif	/* MOTORCONTROL_H */

