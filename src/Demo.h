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

using namespace Basic;

namespace Demo {
constexpr bool enabled = false;

constexpr u1 *const modeLocation = 0;
constexpr u1 modesMax = 2;

void main();

void timeout();

extern void (*timeoutFunc)();

void setTimeoutFunc(void (*tf)());

namespace dumbSpin {
void main();
void timeout();
extern bool go;
}; // namespace dumbSpin

namespace ManualConstantTorque {
void main(u1 ampl = 25);
};

namespace PositionHold {
void main();
}
}; // namespace Demo

}; // namespace ThreePhaseControllerNamespace

#endif /* DEMO_H */
