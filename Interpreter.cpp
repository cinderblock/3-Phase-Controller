
#include "Interpreter.h"
#include "Config.h"
#include "ThreePhaseController.h"
#include "ThreePhaseDriver.h"
#include "Predictor.h"
#include "MLX90363.h"
#include "FilebotInterface/TwillBotInterface.h"
#include "ServoController.h"

void Interpreter::interpretFromMaster(u1 const * const incomingData) {
  if (checkCRC(incomingData)) {
    return;
  }

  if (incomingData[0] == (u1)Command::SetTorque) {
    s2 torque = *((s2*)(incomingData + 1));

    if (torque > ThreePhaseController::getMaxTorque())
      torque = ThreePhaseController::getMaxTorque();
    else if (torque < -ThreePhaseController::getMaxTorque())
      torque = -ThreePhaseController::getMaxTorque();

    ServoController::setTorque(torque);

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

  //deadtime configuration
  if (incomingData[0] == (u1)Command::SetDeadtimes) {
    if (incomingData[1] == 0xF0) {
      //advance to next DeadTime
      ThreePhaseController::setDeadTimes(ThreePhaseController::getDeadTimes() + 0x11);

    } else if (incomingData[1] == 0x0F) {
      //decement to last DeadTime
      ThreePhaseController::setDeadTimes(ThreePhaseController::getDeadTimes() - 0x11);

    } else if (incomingData[1] == 0xFF) {
      ThreePhaseController::setDeadTimes(incomingData[2]);
    }
  }

  if (incomingData[0] == (u1)Command::SetDriverAmplitude) {
    ThreePhaseDriver::setAmplitude(incomingData[1]);
  }

  if (incomingData[0] == (u1)Command::SetDriverPosition) {
    ThreePhaseDriver::advanceTo((((u2)incomingData[2]) << 8) | incomingData[1]);
  }

  if (incomingData[0] == (u1)Command::SetPredictorAdjustVal) {
    Predictor::setAdjustVal(incomingData[1]);
  }

  if (incomingData[0] == (u1)Command::SetPredictorPhaseAdvance) {
    Predictor::setPhaseAdvanceRatio(incomingData[1]);
  }
}

void Interpreter::sendNormalDataToMaster() {

  u1 * const buff = TwillBotInterface::getOutgoingWriteBuffer();

  *(u2 * const)(&buff[0]) = ThreePhaseController::getRoll();
  *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();
  *(u2 * const)(&buff[4]) = ThreePhaseController::getMeasuredPosition();
  *(u2 * const)(&buff[6]) = ServoController::getVelocityCommand();
  // buff[6] = Predictor::getAdjustVal();
  // buff[7] = Predictor::getPhaseAdvanceRatio();
  buff[8] = (u1)ThreePhaseController::getTorque();
  buff[9] = (u1)ThreePhaseController::getDeadTimes();

  TwillBotInterface::writeOutgoingCRC();
  TwillBotInterface::releaseNextWriteBuffer();
}

//check the message to check if it passes CRC check

bool Interpreter::checkCRC(u1 const * const mes) {
  u1 sum = 0;

  for (u1 i = 0; i < Config::i2cBufferIncomingSize; i++)
    //sum = _crc8_ccitt_update(sum, mes[i]);
    sum += mes[i];

  return sum != 0xFF;
}

u1 Interpreter::getCRC(u1 const * const mes, u1 length) {
  u1 sum = 0;

  for (u1 i = 0; i < length; i++)
    //sum = _crc8_ccitt_update(sum, mes[i]);
    sum += mes[i];

  return ~sum;
}
