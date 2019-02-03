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

#include "Config.h"
#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {

namespace Calibration {
using namespace Basic;

constexpr bool enabled = false;

#ifdef BED_CONTROLLER
// 5 mechanical revolutions
constexpr u2 numberOfSpins = 5 * 15;
constexpr u1 amplitude = 60;
#endif

#ifdef QUANTUM_DRIVE
// 10 mechanical revolutions
constexpr u2 numberOfSpins = 7 * 10;
constexpr u1 amplitude = 40;
#endif

constexpr u2 rampSteps = 0x300;
void main();

void step(u4 i);
}; // namespace Calibration

}; // namespace ThreePhaseControllerNamespace

#endif /* CALIBRATION_H */
