/* 
 * File:   ThreePhaseDriver.h
 * Author: Cameron
 *
 * Created on December 17, 2014, 3:20 PM
 */

#ifndef THREEPHASEDRIVER_H
#define	THREEPHASEDRIVER_H

#include "basicTypes.h"

class ThreePhaseDriver {
 static u1 getPhasePWM(u1 const phase) __attribute__((const));
 static u2 const StepsPerPhase = 256;
 static u2 const StepsPerCycle = StepsPerPhase * 3;
public:
 static void init();

};

#endif	/* THREEPHASEDRIVER_H */

