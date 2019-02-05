#pragma once

#include <basicTypes.h>

namespace ThreePhaseControllerNamespace {

using namespace Basic;

enum class State : u1 {
  // Initial mode at startup
  Fault,
  // Collect calibration data (ThreePhaseController not enabled)
  Calibration,
  // Dumb "push" mode of ThreePhaseController (constant force)
  Push,
  // Full "servo" mode handled by ServoController
  Servo,
};

enum class Fault : u1 {
  Init,
  InvalidCommand,
  OverCurrent,
  OverTemperature,
};

extern State state;
extern Fault fault;

bool setState(State s);

} // namespace ThreePhaseControllerNamespace