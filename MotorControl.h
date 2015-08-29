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
    static void go(s1 force);
    static void goAt(s1 speed);
    static void goTo(s2 position);
    static void goDistance(s4 distance);
    
private:
    
    //static u1 stepSize = 1;
    static u4 timeLastStep;

};

#endif	/* MOTORCONTROL_H */

