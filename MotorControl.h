/* 
 * File:   MotorControl.h
 * Author: btacklind
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
    static void advance();
    static void setInitialPosition(u2 pos);
    
    static void go(s1 force);
    static void goAt(s1 speed);
    static void goTo(u2 position);
    static void goDistance(s4 distance);
    
    inline static u2 getTimer(){return TCNT3;};
    
private:
    
    //static u1 stepSize = 1;
    static u2 timeLastStep;
    static u2 lastStep;
    
};

#endif	/* MOTORCONTROL_H */

