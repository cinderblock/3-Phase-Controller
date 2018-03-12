/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Calibration.h
 * Author: Cameron
 *
 * Created on April 11, 2017, 6:49 PM
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

namespace ThreePhaseControllerNamespace {

namespace Calibration {
  constexpr bool enabled = true;
  constexpr float numberOfSpins = 7;
  constexpr uint8_t amplitude = 40;
  constexpr uint16_t rampSteps = 0x300;
  void main();
};

};

#endif /* CALIBRATION_H */

