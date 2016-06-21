
#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "Config.h"

class Interpreter {
  static bool checkCRC(u1 const * const);
  static u1 getCRC(u1 const * const, u1);

  enum class Command : u1 {
    SetTorque = 0x20,
    SetVelocity = 0x21,
	SetPosition = 0x22,
	SetDeadtimes = 0x40,
	Setamplitude,
	SetDriverAmplitude = 0x41,
	SetDriverPosition = 0x42,
	SetPredictorAdjustVal = 0x43,
	SetPredictorPhaseAdvance = 0x44,
  };

public:
  static void interpretFromMaster(u1 const * const);

  static void sendNormalDataToMaster();
};

#endif	/* INTERPRETER_H */
