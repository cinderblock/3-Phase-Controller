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
#include "Debug.h"
#include "commutation.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

volatile u1 HallWatcher::state = 0b111;


void INT6_vect() {
  HallWatcher::checkH1();
  updateCommutation();
}

void PCINT0_vect() {
  HallWatcher::checkH2();
  HallWatcher::checkH3();
  updateCommutation();
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

  checkAndUpdate();
}

void HallWatcher::checkAndUpdate() {
  checkH1();
  checkH2();
  checkH3();
  updateCommutation();
}
