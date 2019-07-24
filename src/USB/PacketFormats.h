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
  Bootloader = 0xfe,
};

/**
 * Shape of data going OUT of host.
 */
typedef struct {
  CommandMode mode;
  union {
    // ThreePhaseCommand
    struct {
      u2 A;
      u2 B;
      u2 C;
    } ThreePhase;

    // CalibrationCommand
    struct {
      u2 angle;
      u1 amplitude;
    } calibrate;

    // PushCommand
    struct {
      s2 command;
    } push;

    // ServoCommand
    struct {
      u1 mode;
      s4 command;
    } servo;
  };
} USBDataOUTShape;

/**
 * Shape of data going IN to host
 */
typedef struct {
  Fault fault : 3;

  u2 position : 13;

  State state : 2;
  u2 cpuTemp : 14;

  s2 current;

  u1 amplitude;

  u1 mlxFailedComms;
} USBDataINShape;

static_assert(sizeof(USBDataOUTShape) <= REPORT_SIZE, "Data going OUT of HOST must be shorter than REPORT_SIZE");
static_assert(sizeof(USBDataINShape) <= REPORT_SIZE, "Data going IN to HOST must be shorter than REPORT_SIZE");

} // namespace ThreePhaseControllerNamespace

#endif /* USBPACKETFORMATS_H */
