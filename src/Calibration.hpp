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
#include "ThreePhase/Driver.hpp"
#include <AVR++/basicTypes.hpp>

namespace ThreePhaseControllerNamespace {

namespace Calibration {
using namespace Basic;

constexpr bool enabled = true;

#ifdef BED_CONTROLLER
// 5 mechanical revolutions
constexpr u2 numberOfSpins = 5 * ThreePhaseDriver::CyclesPerRevolution;
constexpr u1 amplitude = 60;
#endif

#ifdef HOVER_DRIVE
// 5 mechanical revolutions
constexpr u2 numberOfSpins = 5 * ThreePhaseDriver::CyclesPerRevolution;
constexpr u1 amplitude = 20;
#endif

#ifdef QUANTUM_DRIVE
// 10 mechanical revolutions
constexpr u2 numberOfSpins = 10 * ThreePhaseDriver::CyclesPerRevolution;
constexpr u1 amplitude = 40;
#endif

constexpr u2 rampSteps = 0x300;
void main();

void step(u4 i);
}; // namespace Calibration

}; // namespace ThreePhaseControllerNamespace

#endif /* CALIBRATION_H */
