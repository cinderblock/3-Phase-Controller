/* 
 * File:   MotorControl.cpp
 * Author: btacklind
 * 
 * Created on March 28, 2015, 4:30 PM
 */

#include "MotorControl.h"
#include "ThreePhaseDriver.h"


void MotorControl::init(){
}

u2 MotorControl::lastStep;
u2 MotorControl::timeLastStep;

void MotorControl::setInitialPosition(u4 pos){
    
    lastStep = getStepFromLocation(pos);
    timeLastStep = getTimer();
}

u2 MotorControl::getStepFromLocation(u4 magData){
    //should use a look up table based on motor to magnet relation
    return magData;
}

s4 MotorControl::commandVal;
void MotorControl::go(s1 force) {
    
}

MotorControl::Command MotorControl::curCommand;

void MotorControl::goAt(s2 speed) {
    curCommand = Command::Velocity;
    if (speed < 0) speed = -speed;
    if (speed > 15) speed = 15;
    commandVal = speed;
}

void MotorControl::goDistance(s4 distance){
    curCommand = Command::Turns;
    
    if (distance > 0xFF)
        commandVal = 0xFF;
    else if (distance < -0xFF)
        commandVal = -0xFF;
    else 
        commandVal = distance;
    
    s4 nextVal = (lastStep + commandVal);
    
    if (nextVal > 0x2FF) nextVal -= 0x300;
    else if (nextVal < 0) nextVal += 0x300;
    
    ThreePhaseDriver::advanceTo(nextVal);
    lastStep = nextVal;
}

/*
 * Run this function as often as possible.
 * Advances the motor based on an clocked time base allowing for consistent
 * turning speeds
 */
void MotorControl::advance(){
    if (curCommand == Command::Velocity){
        const u2 currTime = getTimer();
        const u2 timeDiff = Timer::getSince(timeLastStep);

        //currently has a hard programmed turn speed
        s2 stepSize = (timeDiff >> commandVal);

        if (stepSize != 0){
            s2 nextStep = lastStep + stepSize;

            if (nextStep > 0x2FF) nextStep -= 0x300;
            else if (nextStep < 0) nextStep += 0x300;

            timeLastStep = currTime;
            lastStep = nextStep;
            ThreePhaseDriver::advanceTo(nextStep);
        }
    }
}



