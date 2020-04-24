/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   USBPacketFormats.h
 * Author: Cameron
 *
 * Created on May 14, 2018, 2:18 PM
 */

#ifndef USBPACKETFORMATS_H
#define USBPACKETFORMATS_H

#include "../MLX90363.hpp"
#include "../main.hpp"
#include "Descriptors.h"
#include "ServoController.hpp"
#include "ThreePhase/Controller.hpp"
#include <stdint.h>

namespace ThreePhaseControllerNamespace {

enum class CommandMode : u1 {
  MLXDebug = 0,
  ThreePhase = 1,
  Calibration = 2,
  Push = 3,
  Servo = 4,
  ClearFault = 5,
  SynchronousDrive = 6,
  Bootloader = 0xfe,
};

typedef struct {
  u1 mlxData[8];
  // Should CRC be calculated locally
  bool crc;
} MLXCommand;

typedef struct {
  u2 A;
  u2 B;
  u2 C;
} ThreePhaseCommand;

typedef struct {
  u2 angle;
  u1 amplitude;
} CalibrationCommand;

typedef struct {
  s2 command;
} PushCommand;

typedef struct {
  bool forward;
  u1 amplitude;
} ServoAmplitudeCommand;

typedef struct {
  // Not yet implemented
} ServoVelocityCommand;

typedef struct {
  u2 commutation;
  s4 turns;
  u2 kP;
  u2 kI;
  u2 kD;
} ServoPositionCommand;

typedef struct {
  ServoController::Mode mode;
  union {
    ServoAmplitudeCommand amplitude;
    ServoVelocityCommand velocity;
    ServoPositionCommand position;
  };
} ServoCommand;

typedef struct {
  u1 amplitude;
  s4 velocity;
} SynchronousCommand;

typedef struct {
  // TODO: fill
  // - amplitudeLimit
  // - deadTimes
  // - Controller phase advance/anti-damping
} ConfigurationCommand;

typedef struct {
  CommandMode mode;
  union {
    MLXCommand mlx;
    ThreePhaseCommand ThreePhase;
    CalibrationCommand calibrate;
    PushCommand push;
    ServoCommand servo;
    SynchronousCommand synchronous;
    ConfigurationCommand configuration;
  };
} CommandFormat;

/**
 * Shape of data going OUT of host.
 */
typedef CommandFormat USBDataOUTShape;

typedef struct {
  Fault fault;

  union {
    struct {
      u1 cyclesPerRevolution;

      bool validCalibration;

      u1 deadTimes;

      u2 currentLimit;

      struct {
        u1 version;
        u8 time;
      } calibration;
    } init; // 14 bytes
  };
} FaultData; // 15 bytes

typedef struct {
  ThreePhaseDriver::PhasePosition drivePosition;
  // If invalid, indicates no calibration
  ThreePhaseDriver::PhasePosition realPosition;
  s4 velocity;
  u1 amplitude;

  MLX90363::ResponseState mlxResponseState;
  u1 mlxResponse[8];
} ManualData; // 18 bytes

typedef struct {
  ServoController::MultiTurn position;
  s2 velocity;
  ThreePhaseController::Amplitude amplitude;

  u2 controlLoops;
  u2 mlxFailedCRCs;
} NormalData; // 14 bytes

typedef struct {
  State state;
  union {
    FaultData fault;
    ManualData manual;
    NormalData normal;
  }; // 18 bytes

  // Analog group. Always share since they're always valid.
  struct {
    u2 cpuTemp;
    s2 current;

    u2 VDD;
    u2 VBatt;

    u2 AS;
    u2 BS;
    u2 CS;
  };          // 14 bytes
} DataFormat; // 33 bytes

/**
 * Shape of data going IN to host
 */
typedef DataFormat USBDataINShape;

static_assert(sizeof(USBDataOUTShape) <= REPORT_SIZE, "Data going OUT of HOST must be shorter than REPORT_SIZE");
static_assert(sizeof(USBDataINShape) <= REPORT_SIZE, "Data going IN to HOST must be shorter than REPORT_SIZE");

} // namespace ThreePhaseControllerNamespace

#endif /* USBPACKETFORMATS_H */
