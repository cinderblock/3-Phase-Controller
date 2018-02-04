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

u1 volatile HallWatcher::state = 0b111;
void (*HallWatcher::stateChangeReceiver)() = nullptr;

void INT0_vect() {
  HallWatcher::checkH1();
}
void INT1_vect() {
  HallWatcher::checkH2();
}
void PCINT0_vect() {
  HallWatcher::checkH3();
}

void HallWatcher::init() {
  // set edge detecction INT6
  EICRB = 0b00010000;
  // Enable INT6
  EIMSK = 0b01000000;

  // Enable PCINT7 & PCINT4
  PCMSK0 = 0b10010000;
  // Enable pin change interupts in general
  PCICR = 0b00000001;

  checkH1();
  checkH2();
  checkH3();
}
