
#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "Config.h"

class Interpreter {

  enum class Command : u1 {
    SetAmplitude = 0x20,
    SetVelocity = 0x21,
    SetPosition = 0x22,
    SetDeadtimes = 0x40,
    SetDriverAmplitude = 0x41,
    SetDriverPosition = 0x42,
    SetPredictorAdjustVal = 0x43,
    SetPredictorPhaseAdvance = 0x44,
  };

  static constexpr u1 extraResponseLength = 10;

  static u1 extraResponse[extraResponseLength];

public:
  static void interpretFromMaster(u1 const * const);

  static void sendNormalDataToMaster();
};

#endif	/* INTERPRETER_H */
