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
#include "Descriptors.h"
#include "main.hpp"
#include <stdint.h>

namespace ThreePhaseControllerNamespace {

enum class CommandMode : u1 { MLXDebug, ThreePhaseDebug, Calibration, Push, Servo };

/**
 * Shape of data going OUT of host.
 */
typedef struct {
  CommandMode mode;
  union {

    // MLXCommand
    struct {
      u1 mlxData[8];
      // Should CRC be calculated locally
      bool crc;
    } mlx;

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
  State state;
  Fault fault;
  u2 position;
  s2 velocity;

  u2 rawAngle;

  u2 cpuTemp;
  s2 current;

  u2 ain0;
  u2 AS;
  u2 BS;
  u2 CS;

  u1 mlxResponse[8];
  MLX90363::ResponseState mlxResponseState;
} USBDataINShape;

static_assert(sizeof(USBDataOUTShape) <= REPORT_SIZE, "Data going OUT of HOST must be shorter than REPORT_SIZE");
static_assert(sizeof(USBDataINShape) <= REPORT_SIZE, "Data going IN to HOST must be shorter than REPORT_SIZE");

} // namespace ThreePhaseControllerNamespace

#endif /* USBPACKETFORMATS_H */
