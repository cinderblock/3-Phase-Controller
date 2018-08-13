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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Shape of data going OUT of host.
 * ( From the rPi).
 */
typedef union {
  struct {
    uint8_t mesgType;
    int16_t command;
  };
  // Force the OUTshape to be 8 bytes long
  uint8_t dummy[8];
} USBDataOUTShape;

/**
 * Shape of data going IN to host
 * ( To the rPi).
 */
typedef struct {
  uint32_t position;
  int16_t velocity;
  uint16_t adc;
} USBDataINShape;

#ifdef __cplusplus
}
#endif

#endif /* USBPACKETFORMATS_H */
