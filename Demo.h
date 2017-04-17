/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Demo.h
 * Author: Cameron
 *
 * Created on April 11, 2017, 6:32 PM
 */

#ifndef DEMO_H
#define DEMO_H

#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {
  
  using namespace AVR;

namespace Demo {
  constexpr bool enabled = true;
  
  constexpr u1 * const modeLocation = 0;
  constexpr u1 modesMax = 0;

  void main();
  
  void timeout();
  
  extern void(*timeoutFunc)();
  
  void setTimeoutFunc(void(*tf)());
  
  namespace dumbSpin {
    void main();
    void timeout();
    extern bool go;
  };
  
  namespace ManualConstantTorque {
    void main();
  };
};

};

#endif /* DEMO_H */

