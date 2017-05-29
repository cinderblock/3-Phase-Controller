
#include "Interpreter.h"
#include "Config.h"
#include "ThreePhaseController.h"
#include "ThreePhaseDriver.h"
#include "ThreePhasePositionEstimator.h"
#include "MLX90363.h"
#include "FilebotInterface.h"
#include "ServoController.h"
#include <CRC8.h>
#include "DriverConstants.h"

using namespace ThreePhaseControllerNamespace;

u1 Interpreter::extraResponse[extraResponseLength];
Interpreter::Mode Interpreter::current;
u1 Interpreter::resolutionShifter;
u2 Interpreter::countIncoming;

Interpreter::PreparedCommand Interpreter::preparedCommand;
u4 Interpreter::preparedValue;

void Interpreter::Init(){
  current = Mode::Standard;
  resolutionShifter = 6;
  countIncoming = 0;
  preparedCommand = PreparedCommand::None;
  preparedValue = 0;
}

void Interpreter::interpretFromMaster(u1 const * const incomingData) {
  using libCameron::CRC8;

  // Check incoming CRC for failure
  CRC8 crc;

  u1 const * data = incomingData;

  for (u1 i = 0; i < CommInterface::incomingBufferSize; i++)
    crc.feed(*data++);

  // If non-zero, CRC fail
  if (crc.getCRC()) return;
  
  countIncoming++;


  if (incomingData[0] == (u1)Command::ExecutePrepared) {
    if(preparedCommand == PreparedCommand::Amplitude){
      ServoController::setAmplitude((s2)preparedValue);
    }
    else if(preparedCommand == PreparedCommand::Velocity){
      ServoController::setVelocity((s2)preparedValue);
    }
    else if(preparedCommand == PreparedCommand::Position){
      ServoController::setPosition((s4)preparedValue);
    }
    else if(preparedCommand == PreparedCommand::Distance){
      ServoController::setDistance((s4)preparedValue);
    }

    return;
  }

  //Set now commands
  if (incomingData[0] == (u1)Command::SetAmplitude) {
    s2 torque = *((s2*)(incomingData + 1));

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

  if(incomingData[0] == (u1)Command::SetDistance){
    s4 dist = *((s4*)(incomingData + 1));

    ServoController::setDistance(dist);

    return;
  }

  //Prepare commands
  if (incomingData[0] == (u1)Command::PrepareAmplitude) {
    preparedValue = (u4)(*((s2*)(incomingData + 1)));
    preparedCommand = PreparedCommand::Amplitude;

    return;
  }

  if (incomingData[0] == (u1)Command::PrepareVelocity) {
    preparedValue = (u4)(*((s2*)(incomingData + 1)));
    preparedCommand = PreparedCommand::Velocity;

    return;
  }

  if (incomingData[0] == (u1)Command::PreparePosition) {
    preparedValue = (u4)(*((s4*)(incomingData + 1)));
    preparedCommand = PreparedCommand::Position;

    return;
  }

  if(incomingData[0] == (u1)Command::PrepareDistance){
    preparedValue = (u4)(*((s4*)(incomingData + 1)));
    preparedCommand = PreparedCommand::Distance;

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
    ThreePhasePositionEstimator::setAdjustVal(incomingData[1]);

    return;
  }

  if (incomingData[0] == (u1)Command::SetPredictorPhaseAdvance) {
    ThreePhasePositionEstimator::setPhaseAdvanceRatio(incomingData[1]);

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
    u1 const len = 4;

    extraResponse[0] = (u1)Command::GetPDSvalues;
    extraResponse[1] = ServoController::getP();
    extraResponse[2] = ServoController::getD();
    extraResponse[3] = ServoController::getShift();
    extraResponse[4] = resolutionShifter;

    crc.reset();

    u1 * data = extraResponse;

    for (u1 i = 0; i < len + headerLen; i++) {
      crc << *data++;
    }

    *data = crc.getCRC();

    CommInterface::setExtraResponse(len + headerLen + 1, extraResponse);

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
    u1 const len = 1;

    extraResponse[0] = (u1)Command::getMode;
    extraResponse[1] = (u1)current;

    crc.reset();

    u1 * data = extraResponse;

    for (u1 i = 0; i < len + headerLen; i++) {
      crc << *data++;
    }

    *data = crc.getCRC();

    CommInterface::setExtraResponse(len + headerLen + 1, extraResponse);

    return;
  }

  if(incomingData[0] == (u1)Command::setPositionResolution){
    resolutionShifter = incomingData[1];
  }

  if (incomingData[0] == (u1)Command::getPositionResolution){
    u1 const headerLen = 1;
    u1 const len = 1;

    extraResponse[0] = (u1)Command::getPositionResolution;
    extraResponse[1] = (u1)resolutionShifter;

    crc.reset();

    u1 * data = extraResponse;

    for (u1 i = 0; i < len + headerLen; i++) {
      crc << *data++;
    }

    *data = crc.getCRC();

    CommInterface::setExtraResponse(len + headerLen + 1, extraResponse);

    return;
  }
}

void Interpreter::sendNormalDataToMaster() {
  u1 * const buff = CommInterface::getOutgoingWriteBuffer();

  static u2 Roll = 0;
  buff[0] = (u1)current;
  //Standard message
  if (current == Mode::Standard){
    *(u2 * const)(&buff[1]) = Roll;
    *(u2 * const)(&buff[3]) = ThreePhaseController::getVelocity();
    *(s2 * const)(&buff[5]) = getPosition();
    *(u2 * const)(&buff[7]) = countIncoming;
    buff[9] = (u1)ThreePhaseController::getAmplitude();
    *(u4 * const)(&buff[10]) = ServoController::getPositionCommand();
  }
  else if(current == Mode::Calibration){
    *(u2 * const)(&buff[1]) = Roll;
    *(u2 * const)(&buff[3]) = ThreePhaseController::getLastAplha();
    // *(u4 * const)(&buff[0]) = Predictor::getPhaseAdvanceAmount();
    // *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
//    *(u2 * const)(&buff[5]) = ThreePhaseDriver::lastPhase;
    // *(u2 * const)(&buff[6]) = Predictor::getPredictedPosition();
    // *(s4 * const)(&buff[4]) = ServoController::getPosition();
    buff[8] = (u1)ThreePhaseController::getAmplitude();
  }
  else if(current == Mode::Test){
    *(u2 * const)(&buff[1]) = Roll;
    *(u2 * const)(&buff[3]) = ThreePhaseController::getVelocity();
    *(s2 * const)(&buff[5]) = getPosition();
    *(u2 * const)(&buff[7]) = countIncoming;
    buff[9] = (u1)ThreePhaseController::getAmplitude();
    *(u4 * const)(&buff[10]) = ServoController::getPositionCommand();
    // *(u2 * const)(&buff[0]) = ThreePhaseController::getMeasuredPosition();
    // *(u4 * const)(&buff[0]) = Predictor::getPhaseAdvanceAmount();
    // *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
    // *(u4 * const)(&buff[0]) = ServoController::getPosition();
    // *(u4 * const)(&buff[4]) = ServoController::getPositionCommand();
    // *(s1 * const)(&buff[8]) = (s1)ServoController::getRevolution();
    // *(u2 * const)(&buff[4]) = ThreePhaseDriver::lastPhase;
    // *(u2 * const)(&buff[6]) = Predictor::getPredictedPosition();
    // *(s4 * const)(&buff[4]) = ServoController::getPosition();
    // buff[8] = (u1)ThreePhaseController::getAmplitude();
  }

  Roll++;
  CommInterface::writeOutgoingCRC();
  CommInterface::releaseNextWriteBuffer();
}
