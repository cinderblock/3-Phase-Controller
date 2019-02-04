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

#include "Descriptors.h"
#include "main.h"
#include <stdint.h>

namespace ThreePhaseControllerNamespace {

using reportSize = uint8_t[REPORT_SIZE];

/**
 * Shape of data going OUT of host.
 * ( From the rPi).
 */
typedef union {
  struct {
    uint8_t mesgType;
    int32_t command;
  };
  // Force the OUTshape to be 8 bytes long
  reportSize rawReport;
} USBDataOUTShape;

/**
 * Shape of data going IN to host
 * ( To the rPi).
 */
typedef union {
  struct {
    statusType status;
    uint32_t position;
    int16_t velocity;
    uint16_t cpuTemp;
    int16_t current;
    uint16_t rawAngle;
  };
  // Force the OUTshape to be 8 bytes long
  reportSize rawReport;
} USBDataINShape;

static_assert(sizeof(USBDataOUTShape) <= REPORT_SIZE, "");
static_assert(sizeof(USBDataINShape) <= REPORT_SIZE, "");

} // namespace ThreePhaseControllerNamespace

#endif /* USBPACKETFORMATS_H */
