/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreePhaseFaultTester.h
 * Author: Cameron
 *
 * Created on April 16, 2018, 12:58 PM
 */

#ifndef THREEPHASEFAULTTESTER_H
#define THREEPHASEFAULTTESTER_H

#include "ThreePhaseDriver.h"

namespace ThreePhaseControllerNamespace {

  class ThreePhaseFaultTester {
  public:
    static void init();

    enum class FailMode : u1 {
      Pass, Fail,
    };

    enum class Channel: u1 {
      AL, BL, CL, AH, BH, CH,
    };

    static FailMode testPhase(ThreePhaseDriver::Phase p);

    static FailMode testChannel(Channel c);
  private:

  };

}

#endif /* THREEPHASEFAULTTESTER_H */

