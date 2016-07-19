
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

  if (incomingData[0] == (u1)Command::SetPDSvalues) {
    ServoController::setP(incomingData[1]);
    ServoController::setPshift(incomingData[2]);
    ServoController::setD(incomingData[3]);
    ServoController::setDshift(incomingData[4]);
  }
}

void Interpreter::sendNormalDataToMaster() {
  u1 * const buff = TwillBotInterface::getOutgoingWriteBuffer();

  // *(u2 * const)(&buff[0]) = ThreePhaseController::getRoll();
  // *(u2 * const)(&buff[2]) = ThreePhaseController::getVelocity();

  buff[0] = (u1)ServoController::getP();
  buff[1] = (u1)ServoController::getD();
  buff[2] = (u1)ServoController::getPshift();
  buff[3] = (u1)ServoController::getDshift();
  // *(s4 * const)(&buff[6]) = ServoController::getPositionCommand();


  // *(u2 * const)(&buff[4]) = DriverConstants::MagnetometerMax - MLX90363::getAlpha();
  // *(u2 * const)(&buff[6]) = ServoController::getRevolution();
  *(s4 * const)(&buff[4]) = ServoController::getPosition();
  // buff[6] = Predictor::getAdjustVal();
  // buff[7] = Predictor::getPhaseAdvanceRatio();
  buff[8] = (u1)ThreePhaseController::getAmplitude();
  buff[9] = (u1)TwillBotInterface::status;

  TwillBotInterface::writeOutgoingCRC();
  TwillBotInterface::releaseNextWriteBuffer();
}
