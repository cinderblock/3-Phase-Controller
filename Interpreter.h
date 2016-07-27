
#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "Config.h"

class Interpreter {
  enum class Mode : u1{
    Standard = 0x01, 
    Calibration = 0x02,
    Test = 0x03,
  };

  static constexpr u1 extraResponseLength = Config::i2cExtraResponseMaxLength;
  static u1 extraResponse[extraResponseLength];
  static Mode current;

  enum class Command : u1 {
    SetAmplitude = 0x20,
    SetVelocity = 0x21,
    SetPosition = 0x22,
    SetDeadtimes = 0x40,
    SetDriverAmplitude = 0x41,
    SetDriverPosition = 0x42,
    SetPredictorAdjustVal = 0x43,
    SetPredictorPhaseAdvance = 0x44,
    SetPDSvalues = 0x50,
    GetPDSvalues = 0x51,
    setMode = 0x01,
    getMode = 0x02,
  };

public:
  static void interpretFromMaster(u1 const * const);

  static void sendNormalDataToMaster();

  static void Init();
};

#endif	/* INTERPRETER_H */
