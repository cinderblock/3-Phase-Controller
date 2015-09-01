/* 
 * File:   Timer.cpp
 * Author: Blake Tacklind
 *
 * Created on August 31, 2015, 5:55 PM
 */

#include "Timer.h"

//const b3 Timer::CLKprescale;

void Timer::init(){
    
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
  * 0        0                0        0        -       -      -
  * Noise   |Edge            | Normal          | Clock
  * Canceler|Select          | Mode            | user defined
  */
 TCCR3B = 0b00000000 & CLKprescale;
 
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

const u2 Timer::prescaleDiv(){
    switch(CLKprescale){
        case 0b001: return 1;
        case 0b010: return 8;
        case 0b011: return 64;
        case 0b100: return 256;
        case 0b101: return 1024;
        default: return 0;
    }
}

u2 Timer::getSince(const u2 time){
    u2 t = getCurTime();
    if(time < t) return t - time;
    else return t + (0xFFFF - time);
}

const u2 Timer::lengthUS(const u4 us){
    return cyclesPerUS * us / prescaleDiv();
}

const u2 Timer::lengthMS(const u2 ms){
    return cyclesPerUS * ms * 1000 / prescaleDiv();
}
