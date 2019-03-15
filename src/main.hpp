#pragma once

#include <AVR++/Atomic.hpp>
#include <basicTypes.hpp>

namespace ThreePhaseControllerNamespace {

using namespace Basic;

void bootloaderJump();

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
  WatchdogReset,
  BrownOutReset,
  InvalidCommand,
};

extern State state;
extern Fault fault;

bool setState(State s);

void clearFault();

struct ResetCauses {
  union {
    struct {
      bool PowerOn : 1;
      bool External : 1;
      bool BrownOut : 1;
      bool WatchDog : 1;
      bool JTAG : 1;
      bool USB : 1;
    };
    u1 byte;
  };

  inline ResetCauses(u1 v = MCUSR) : byte(v) {}
};
extern const ResetCauses resetCauses;

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

namespace ADCValues {
extern volatile Atomic<u2> AS;
extern volatile Atomic<u2> BS;
extern volatile Atomic<u2> CS;
extern volatile Atomic<u2> current;
extern volatile Atomic<u2> currentRef;
extern volatile Atomic<u2> battery;
extern volatile Atomic<u2> drive;
extern volatile Atomic<u2> temperature;

} // namespace ADCValues

} // namespace ThreePhaseControllerNamespace