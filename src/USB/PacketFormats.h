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

#include "../PacketFormats.h"
#include "Descriptors.h"
#include <stdint.h>

namespace ThreePhaseControllerNamespace {

/**
 * Shape of data going OUT of host.
 */
typedef CommandFormat USBDataOUTShape;

/**
 * Shape of data going IN to host
 */
typedef DataFormat USBDataINShape;

static_assert(sizeof(USBDataOUTShape) <= REPORT_SIZE, "Data going OUT of HOST must be shorter than REPORT_SIZE");
static_assert(sizeof(USBDataINShape) <= REPORT_SIZE, "Data going IN to HOST must be shorter than REPORT_SIZE");
} // namespace ThreePhaseControllerNamespace

#endif /* USBPACKETFORMATS_H */
