/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HallWatcher.cpp
 * Author: Cameron
 * 
 * Created on October 12, 2017, 1:53 PM
 */

#include "HallWatcher.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

u1 HallWatcher::state = 0b111;

void INT6_vect() {
  HallWatcher::checkH1();
}
void PCINT0_vect() {
  HallWatcher::checkH2();
  HallWatcher::checkH3();
}

void HallWatcher::init() {
  // TODO: Enable INT6, PCINT4, & PCINT7
}
