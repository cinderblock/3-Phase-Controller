#include "Interface.hpp"
#include <AVR++/WDT.hpp>
#include <string.h>

using namespace ThreePhaseControllerNamespace;

void ThreePhaseControllerNamespace::handleIncomingCommand(const CommandFormat &data) {

  switch (data.mode) {
  default:
    return;

  case CommandMode::ClearFault:
    WDT::stop();
    clearFault();
    break;

  case CommandMode::MLXDebug:
    if (state == State::Fault && fault != Fault::Init)
      return;

    if (setState(State::Manual))
      WDT::stop();

    memcpy(MLX90363::getTxBuffer(), data.mlx.mlxData, MLX90363::messageLength);
    if (data.mlx.crc)
      MLX90363::fillTxBufferCRC();

    MLX90363::startTransmittingUnsafe();
    break;

  case CommandMode::ThreePhase:
    if (state == State::Fault && fault != Fault::Init)
      return;

    if (setState(State::Manual))
      WDT::start(WDT::T1000ms);

    // TODO: Implement body of this "method"
    break;

  case CommandMode::Calibration:
    if (state == State::Fault && fault != Fault::Init)
      return;

    if (setState(State::Manual))
      WDT::start(WDT::T1000ms);

    ThreePhaseDriver::setAmplitude(data.calibrate.amplitude);
    ThreePhaseDriver::advanceTo(data.calibrate.angle);
    break;

  case CommandMode::Push:
    if (state == State::Fault && fault != Fault::Init)
      return;

    if (setState(State::Normal))
      WDT::start(WDT::T_120ms);

    ServoController::setEnable(false);
    ThreePhaseController::setAmplitudeTarget(data.push.command);
    break;

  case CommandMode::Servo:
    if (state == State::Fault && fault != Fault::Init)
      return;

    if (setState(State::Normal))
      WDT::start(WDT::T_250ms);

    switch (data.servo.mode) {
    default:
      return;

    case ServoController::Mode::Disabled:
      ServoController::setEnable(false);
      break;

    case ServoController::Mode::Amplitude: {
      const auto &parameters = data.servo.amplitude;
      ServoController::setAmplitude(ThreePhaseController::Amplitude(parameters.forward, parameters.amplitude));
    } break;

    case ServoController::Mode::Velocity:
      // TODO: Implement
      ServoController::setVelocity(0);
      break;

    case ServoController::Mode::Position: {
      const auto &parameters = data.servo.position;
      ServoController::setPosition(parameters.turns);
      ServoController::setPositionK(parameters.pid);

      ServoController::setVelocityLimit(parameters.limits.velocity.upper, parameters.limits.velocity.lower);

      const ThreePhaseController::Amplitude a(parameters.limits.push.upperForward,
                                              parameters.limits.push.upperAmplitude);

      const ThreePhaseController::Amplitude b(parameters.limits.push.lowerForward,
                                              parameters.limits.push.lowerAmplitude);

      ServoController::setAmplitudeLimit(a, b);

    } break;
    }

    break;

  case CommandMode::SynchronousDrive:
    if (setState(State::Manual))
      WDT::start(WDT::T_500ms);

    ThreePhaseControllerNamespace::setSynchronous(data.synchronous.velocity);
    ThreePhaseDriver::setAmplitude(data.synchronous.amplitude);
    break;

  case CommandMode::Bootloader:
    ThreePhaseDriver::emergencyDisable();
    WDT::stop();
    ServoController::setEnable(false);
    bootloaderJump();
    break;
  }

  WDT::tick();
}

void ThreePhaseControllerNamespace::packOutputDataPacket(DataFormat &data) {

  data.state = state;
  switch (data.state) {
  case State::Fault:
    data.fault.fault = fault;

    if (fault == Fault::Init) {
      data.fault.init.cyclesPerRevolution = ThreePhaseDriver::CyclesPerRevolution;

      data.fault.init.deadTimes = DT4;

      // TODO:
      data.fault.init.currentLimit = 0;

      data.fault.init.validCalibration = Lookup::isValid;
      if (Lookup::isValid) {
        // TODO:
        // data.fault.init.calibration.time = ;
        // data.fault.init.calibration.version = ;
      }
    }
    break;

  case State::Manual:
    data.manual.drivePosition = ThreePhaseControllerNamespace::getManualPosition();
    data.manual.realPosition = Lookup::isValid ? ThreePhasePositionEstimator::getMagnetometerPhaseEstimate() : true;
    data.manual.velocity = ThreePhaseControllerNamespace::getSynchronous();
    data.manual.amplitude = ThreePhaseDriver::getAmplitude();

    data.manual.mlxResponseState = MLX90363::getResponseState();
    memcpy(data.manual.mlxResponse, MLX90363::getRxBuffer(), MLX90363::messageLength);

    break;
  case State::Normal:
    data.normal.position = ServoController::getPosition();
    data.normal.velocity = ThreePhasePositionEstimator::getMagnetometerVelocityEstimate();

    data.normal.amplitude = ThreePhaseController::getAmplitudeTarget();

    data.normal.mlxFailedCRCs = MLX90363::getCRCFailures();
    data.normal.controlLoops = ThreePhaseController::getLoopCount();
    break;
  }

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    data.cpuTemp = ADCValues::temperature.getUnsafe();
    data.current = ADCValues::current.getUnsafe();
    data.VBatt = ADCValues::battery.getUnsafe();
    data.VDD = ADCValues::drive.getUnsafe();
    data.AS = ADCValues::AS.getUnsafe();
    data.BS = ADCValues::BS.getUnsafe();
    data.CS = ADCValues::CS.getUnsafe();
  }
}
