/* 
 * File:   MotorControl.h
 * Author: btacklind
 *
 * Created on March 28, 2015, 4:30 PM
 */

#ifndef MOTORCONTROL_H
#define	MOTORCONTROL_H

#include "ThreePhaseDriver.h"
#include "Timer.h"
/*
 * Uses 16-bit timer 3 to have a consistent time base for motor driving
 * 
 */

class MotorControl {
public:
    static void init();
    static void advance();
    //should only be run once at the initialization of motor driver
    static void setInitialPosition(u4 pos);
    
    static void go(s1 force);
    static void goAt(s2 speed);
    static void goTo(u2 position);
    static void goDistance(s4 distance);
    
    inline static u2 getTimer(){return Timer::getCurTime();};
    inline static u2 getStep(){return lastStep;};
    
private:
    
    //Based on the position of the motor and attached magnet
    //values (should be) configured in local config
    //This could be a look-up table
    static u2 getStepFromLocation(u4 magData);
    
    //static u1 stepSize = 1;
    static u2 timeLastStep;
    static u2 lastStep;
    
    enum class Command : u1 {
        Acceleration, Velocity, Position, Turns
    };
    
};

#endif	/* MOTORCONTROL_H */

