/* 
 * File:   TimerTimeout.cpp
 * Author: Cameron
 * 
 * Created on April 1, 2016, 3:47 PM
 */

#include <avr/io.h>
#include <util/atomic.h>

#include "TimerTimeout.h"

void TimerTimeout::init() {
 static bool initted = false;
 if (initted) return;
 initted = true;

 TCCR0B = 0;
 TIMSK0 = 0;
 TIFR0 = 0xff;
 
 // Setup TIMER0
 TCCR0A = 0;
 TCCR0B = timerClockSelect;
}

void TimerTimeout::startA(u1 period) {
 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  startAISR(period);
 }
}

void TimerTimeout::startB(u1 period) {
 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  startBISR(period);
 }
}

void TimerTimeout::startAISR(u1 period) {
 OCR0A = TCNT0 + period;
 TIFR0 = 1 << OCF0A;
 TIMSK0 |= 1 << OCIE0A;
}

void TimerTimeout::startBISR(u1 period) {
 OCR0B = TCNT0 + period;
 TIFR0 = 1 << OCF0B;
 TIMSK0 |= 1 << OCIE0B;
}

void TimerTimeout::stopA() {
 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  stopBISR();
 }
}

void TimerTimeout::stopB() {
 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  stopBISR();
 }
}

void TimerTimeout::stopAISR() {
 TIMSK0 &= ~(1 << OCIE0A);
}

void TimerTimeout::stopBISR() {
 TIMSK0 &= ~(1 << OCIE0A);
}