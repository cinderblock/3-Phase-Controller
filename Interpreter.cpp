
#include "Interpreter.h"
#include "Config.h"
#include "ThreePhaseController.h"
#include "ThreePhaseDriver.h"
#include "Predictor.h"
#include "MLX90363.h"
#include "FilebotInterface/TwillBotInterface.h"
#include "ServoController.h"
#include <AVR++/CRC8.h>
#include "DriverConstants.h"

u1 Interpreter::extraResponse[extraResponseLength];
Interpreter::Mode Interpreter::current;

void Interpreter::Init(){
  current = Mode::Standard;
}

void Interpreter::interpretFromMaster(u1 const * const incomingData) {
  // Check incoming CRC for failure
  CRC8 crc;

  u1 const * data = incomingData;

  for (u1 i = 0; i < TwillBotInterface::incomingBufferSize; i++)
    crc.feed(*data++);

  // If non-zero, CRC fail
  if (crc.getCRC()) return;

  if (incomingData[0] == (u1)Command::SetAmplitude) {
    s2 torque = *((s2*)(incomingData + 1));

    if (torque > ThreePhaseController::getMaxAmplitude())
      torque = ThreePhaseController::getMaxAmplitude();
    else if (torque < -ThreePhaseController::getMaxAmplitude())
      torque = -ThreePhaseController::getMaxAmplitude();

    ServoController::setAmplitude(torque);

    return;
  }

  if (incomingData[0] == (u1)Command::SetVelocity) {
    s2 velocity = *((s2*)(incomingData + 1));

    ServoController::setVelocity(velocity);

    return;
  }

  if (incomingData[0] == (u1)Command::SetPosition) {
    s4 pos = *((s4*)(incomingData + 1));

    ServoController::setPosition(pos);

    return;
  }

  // deadtime configuration
  if (incomingData[0] == (u1)Command::SetDeadtimes) {
    if (incomingData[1] == 0xF0) {
      //advance to next DeadTime
      ThreePhaseController::setDeadTimes(ThreePhaseController::getDeadTimes() + 0x11);

    } else if (incomingData[1] == 0x0F) {
      // decement to last DeadTime
      ThreePhaseController::setDeadTimes(ThreePhaseController::getDeadTimes() - 0x11);

    } else if (incomingData[1] == 0xFF) {
      ThreePhaseController::setDeadTimes(incomingData[2]);
    }

    return;
  }

  if (incomingData[0] == (u1)Command::SetDriverAmplitude) {
    ServoController::setPhaseMode();
    ThreePhaseDriver::setAmplitude(incomingData[1]);

    return;
  }

  if (incomingData[0] == (u1)Command::SetDriverPosition) {
    ServoController::setPhaseMode();
    ThreePhaseDriver::advanceTo((((u2)incomingData[2]) << 8) | (u2)incomingData[1]);
    return;
  }

  if (incomingData[0] == (u1)Command::SetPredictorAdjustVal) {
    Predictor::setAdjustVal(incomingData[1]);

    return;
  }

  if (incomingData[0] == (u1)Command::SetPredictorPhaseAdvance) {
    Predictor::setPhaseAdvanceRatio(incomingData[1]);

    return;
  }

  if (incomingData[0] == (u1)Command::SetPDSvalues) {
    ServoController::setP(incomingData[1]);
    ServoController::setD(incomingData[2]);
    ServoController::setShift(incomingData[3]);

    return;
  }

  if (incomingData[0] == (u1)Command::GetPDSvalues){

    u1 const headerLen = 1;
    u1 const len = 3;

    extraResponse[0] = (u1)Command::GetPDSvalues;
    extraResponse[1] = ServoController::getP();
    extraResponse[2] = ServoController::getD();
    extraResponse[3] = ServoController::getShift();

    crc.reset();

    u1 * data = extraResponse;

    for (u1 i = 0; i < len + headerLen; i++) {
      crc << *data++;
    }

    *data = crc.getCRC();

    TwillBotInterface::setExtraResponse(len + headerLen + 1, extraResponse);

    return;
  }

  if (incomingData[0] == (u1)Command::setMode){
    switch(incomingData[1]){
      case (u1)Mode::Calibration:
        current = Mode::Calibration;
        break;
      case (u1)Mode::Test:
        current = Mode::Test;
        break;
      default: current = Mode::Standard;
    }
    // current = (Mode)incomingData[1];
    return;
  }

  if (incomingData[0] == (u1)Command::getMode){
    u1 const headerLen = 1;
    u1 const len = 4;

    extraResponse[0] = (u1)Command::GetPDSvalues;
    extraResponse[1] = (u1)current;

    crc.reset();

    u1 * data = extraResponse;

    for (u1 i = 0; i < len + headerLen; i++) {
      crc << *data++;
    }

    *data = crc.getCRC();

    TwillBotInterface::setExtraResponse(len + headerLen + 1, extraResponse);

    return;
  }
}

void Interpreter::sendNormalDataToMaster() {
  u1 * const buff = TwillBotInterface::getOutgoingWriteBuffer();

  static u2 Roll = 0;
  buff[9] = (u1)current;
  //Standard message
  if (current == Mode::Standard){
    *(u2 * const)(&buff[0]) = Roll;
    *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
    // *(s4 * const)(&buff[4]) = ServoController::getPosition();
    *(u2 * const)(&buff[6]) = Predictor::getPredictedPosition();
    buff[8] = (u1)ThreePhaseController::getAmplitude();
  }
  else if(current == Mode::Calibration){
    *(u2 * const)(&buff[0]) = Roll;
    *(u2 * const)(&buff[2]) = ThreePhaseController::getMeasuredPosition();
    // *(u4 * const)(&buff[0]) = Predictor::getPhaseAdvanceAmount();
    // *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
    *(u2 * const)(&buff[4]) = ThreePhaseDriver::lastPhase;
    // *(u2 * const)(&buff[6]) = Predictor::getPredictedPosition();
    // *(s4 * const)(&buff[4]) = ServoController::getPosition();
    buff[8] = (u1)ThreePhaseController::getAmplitude();
  }
  else if(current == Mode::Test){
    *(u2 * const)(&buff[0]) = ThreePhaseController::getMeasuredPosition();
    // *(u4 * const)(&buff[0]) = Predictor::getPhaseAdvanceAmount();
    *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
    *(u2 * const)(&buff[4]) = ThreePhaseDriver::lastPhase;
    *(u2 * const)(&buff[6]) = Predictor::getPredictedPosition();
    // *(s4 * const)(&buff[4]) = ServoController::getPosition();
    buff[8] = (u1)ThreePhaseController::getAmplitude();
  }

  Roll++;
  TwillBotInterface::writeOutgoingCRC();
  TwillBotInterface::releaseNextWriteBuffer();
}
