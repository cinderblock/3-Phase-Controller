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
 static u2 getPhasePWM(u1 const phase) __attribute__((const));
public:
 static void init();

};

#endif	/* THREEPHASEDRIVER_H */

