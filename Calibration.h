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

#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {

namespace Calibration {
using namespace Basic;

constexpr bool enabled = false;
constexpr u2 numberOfSpins = 7 * 5;
constexpr u1 amplitude = 70;
constexpr u2 rampSteps = 0x300;
void main();

void step(u4 i);
}; // namespace Calibration

}; // namespace ThreePhaseControllerNamespace

#endif /* CALIBRATION_H */
