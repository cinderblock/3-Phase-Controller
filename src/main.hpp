#pragma once

#include <basicTypes.hpp>

namespace ThreePhaseControllerNamespace {

using namespace Basic;

enum class State : u1 {
  // Initial mode at startup
  Fault,
  // Manual control over PWMs amplitudes and MLX Data (For calibration and configuration)
  Manual,
  // Full "servo" mode handled by ServoController
  Normal,
};

enum class Fault : u1 {
  Init,
  UndervoltageLockout,
  OverCurrent,
  OverTemperature,
};

extern State state;
extern Fault fault;

bool setState(State s);

extern const u1 resetCause;

namespace Analog {
void temperature();
void current();
void currentRef();
void battery();
void drive();
void AS();
void BS();
void CS();
} // namespace Analog

} // namespace ThreePhaseControllerNamespace