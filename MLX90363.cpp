/*
 * File:   MLX90363.cpp
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:11 PM
 */

#include <avr/pgmspace.h>

#include "MLX90363.h"
#include "Board.h"
#include "TwillBotInterface.h"

#include <avr/interrupt.h>
#include <AVR++/SPI.h>

static inline void sendSPI(u1 const b) {
 *AVR::SPI::DR = b;
}

static inline u1 receiveSPI() {
 return *AVR::SPI::DR;
}

static inline void slaveDeselect() {PORTD |=  (1<<5);}
static inline void slaveSelect  () {PORTD &= ~(1<<5);}

/**
 * Declare the SPI Transfer Complete interrupt as a non-blocking interrupt so
 * that the motor driver has minimum latency.
 *
 * gcc simply enables interrupts at the beginning of the routine allowing any
 * interrupt to fire again. We can trust that this one will not fire twice
 * accidentally by not starting a serial transfer until the end of the interrupt.
 *
 * This is not a guarantee, but it will be good enough if we leave enough stack
 * room.
 */
ISR (SPI_STC_vect, ISR_NOBLOCK) {
 MLX90363::isr();
}

void MLX90363::isr() {
 // Receive a byte
 MLX90363::RxBuffer[MLX90363::bufferPosition++] = receiveSPI();

 // Check if we're done sending
 if (MLX90363::bufferPosition == MLX90363::messageLength) {
  // We're done. De-assert (turn on) the slave select line
  slaveDeselect();
  
  responseState = ResponseState::Received;
  
  // Check the received CRC and read values
  handleResponse();
  
 } else {
  sendSPI(MLX90363::TxBuffer[MLX90363::bufferPosition]);
  /**
   * Once this gets sent, there is a chance of nesting deep on the stack.
   * Luckily we're, in this particular usage, limited to transmissions of 8 bytes
   * at a time. Therefore, this whole could only ever worst case nest with 7 layers.
   * Looking at the assembly code, we can see that the interrupt uses 8 bytes of
   * stack, add the 3 for the interrupt return location for each, and we're at
   * 77 bytes of stack maximum.
   *
   * Add the worst case of another interrupt happening, to get the real worst
   * case stack usage.
   */
 }
}

u1 MLX90363::TxBuffer[messageLength];
u1 MLX90363::RxBuffer[messageLength];
u1 MLX90363::bufferPosition = messageLength;

MLX90363::ResponseState MLX90363::responseState = MLX90363::ResponseState::Ready;


u2 MLX90363::alpha;
u2 MLX90363::beta;
u2 MLX90363::X;
u2 MLX90363::Y;
u2 MLX90363::Z;

u1 MLX90363::err;
u1 MLX90363::VG;
u1 MLX90363::ROLL;


void MLX90363::init() {
 // Setup Slave Select line
 slaveDeselect();
 SS.output();

 // Setup "User Defined" hardware lines
 SCLK.output();
 MOSI.output();
 // Don't forget the AVR's hardware SS line!

 // SPI hardware does this for us, but do it anyway
 SCLK.off();
 MISO.input();
 MISO.on();

 // Setup control registers
 AVR::SPI::SR->byte = 0;
 // F_CPU/32 by default
 AVR::SPI::CR->byte = 0b11010110;

 responseState = ResponseState::Ready;
}

void MLX90363::setSPISpeed(const u1 c) {
 AVR::SPI::CR->Divider = c;
}

void MLX90363::startTransmittingUnsafe() {
 bufferPosition = 0;
 slaveSelect();
 sendSPI(TxBuffer[bufferPosition]);
 responseState = ResponseState::Receiving;
}

/**
 * Lookup table from MLX90363 datasheet
 *
 * The name cba_256_TAB comes from the datasheet as well
 */
static const u1 cba_256_TAB[] PROGMEM = {
 0x00,0x2f,0x5e,0x71,0xbc,0x93,0xe2,0xcd,0x57,0x78,0x09,0x26,0xeb,0xc4,0xb5,0x9a,
 0xae,0x81,0xf0,0xdf,0x12,0x3d,0x4c,0x63,0xf9,0xd6,0xa7,0x88,0x45,0x6a,0x1b,0x34,
 0x73,0x5c,0x2d,0x02,0xcf,0xe0,0x91,0xbe,0x24,0x0b,0x7a,0x55,0x98,0xb7,0xc6,0xe9,
 0xdd,0xf2,0x83,0xac,0x61,0x4e,0x3f,0x10,0x8a,0xa5,0xd4,0xfb,0x36,0x19,0x68,0x47,
 0xe6,0xc9,0xb8,0x97,0x5a,0x75,0x04,0x2b,0xb1,0x9e,0xef,0xc0,0x0d,0x22,0x53,0x7c,
 0x48,0x67,0x16,0x39,0xf4,0xdb,0xaa,0x85,0x1f,0x30,0x41,0x6e,0xa3,0x8c,0xfd,0xd2,
 0x95,0xba,0xcb,0xe4,0x29,0x06,0x77,0x58,0xc2,0xed,0x9c,0xb3,0x7e,0x51,0x20,0x0f,
 0x3b,0x14,0x65,0x4a,0x87,0xa8,0xd9,0xf6,0x6c,0x43,0x32,0x1d,0xd0,0xff,0x8e,0xa1,
 0xe3,0xcc,0xbd,0x92,0x5f,0x70,0x01,0x2e,0xb4,0x9b,0xea,0xc5,0x08,0x27,0x56,0x79,
 0x4d,0x62,0x13,0x3c,0xf1,0xde,0xaf,0x80,0x1a,0x35,0x44,0x6b,0xa6,0x89,0xf8,0xd7,
 0x90,0xbf,0xce,0xe1,0x2c,0x03,0x72,0x5d,0xc7,0xe8,0x99,0xb6,0x7b,0x54,0x25,0x0a,
 0x3e,0x11,0x60,0x4f,0x82,0xad,0xdc,0xf3,0x69,0x46,0x37,0x18,0xd5,0xfa,0x8b,0xa4,
 0x05,0x2a,0x5b,0x74,0xb9,0x96,0xe7,0xc8,0x52,0x7d,0x0c,0x23,0xee,0xc1,0xb0,0x9f,
 0xab,0x84,0xf5,0xda,0x17,0x38,0x49,0x66,0xfc,0xd3,0xa2,0x8d,0x40,0x6f,0x1e,0x31,
 0x76,0x59,0x28,0x07,0xca,0xe5,0x94,0xbb,0x21,0x0e,0x7f,0x50,0x9d,0xb2,0xc3,0xec,
 0xd8,0xf7,0x86,0xa9,0x64,0x4b,0x3a,0x15,0x8f,0xa0,0xd1,0xfe,0x33,0x1c,0x6d,0x42,
};

/**
 * Lookup the next crc value from the above table
 * @param b
 * @return
 */
static u1 lookupCRC(u1 const b) __attribute__((const));
inline static u1 lookupCRC(u1 const b) {
 return pgm_read_byte(&cba_256_TAB[b]);
}

bool MLX90363::checkRxBufferCRC() {
 u1 crc = 0xff;

 crc = lookupCRC(RxBuffer[0] ^ crc);
 crc = lookupCRC(RxBuffer[1] ^ crc);
 crc = lookupCRC(RxBuffer[2] ^ crc);
 crc = lookupCRC(RxBuffer[3] ^ crc);
 crc = lookupCRC(RxBuffer[4] ^ crc);
 crc = lookupCRC(RxBuffer[5] ^ crc);
 crc = lookupCRC(RxBuffer[6] ^ crc);

 return RxBuffer[7] == (u1)~crc;
}

void MLX90363::fillTxBufferCRC() {
 u1 crc = 0xff;

 crc = lookupCRC(TxBuffer[0] ^ crc);
 crc = lookupCRC(TxBuffer[1] ^ crc);
 crc = lookupCRC(TxBuffer[2] ^ crc);
 crc = lookupCRC(TxBuffer[3] ^ crc);
 crc = lookupCRC(TxBuffer[4] ^ crc);
 crc = lookupCRC(TxBuffer[5] ^ crc);
 crc = lookupCRC(TxBuffer[6] ^ crc);

 TxBuffer[7] = ~crc;
}

void MLX90363::handleResponse() {
 responseState = ResponseState::Receiving;
 
 if (!checkRxBufferCRC()) {
  responseState = ResponseState::failedCRC;
  return;
 }
 
 u1 const marker = RxBuffer[6] >> 6;

 if (marker == 0) {
  handleAlpha();
  responseState = ResponseState::TypeA;
  return;
 }
 
 if (marker == 1) {
  handleAlphaBeta();
  responseState = ResponseState::TypeAB;
  return;
 }

 if (marker == 2) {
  handleXYZ();
  responseState = ResponseState::TypeXYZ;
  return;
 }

 responseState = ResponseState::Other;
}

b6 MLX90363::getReceivedOpCode() {
 return RxBuffer[6] & 0x3f;
}


void MLX90363::handleAlpha() {
 alpha = RxBuffer[0] | (RxBuffer[1] & 0x3f) << 8;
 
 err   = RxBuffer[1] >> 6;
 VG    = RxBuffer[4];
 ROLL = RxBuffer[6] & 0x3f;
}

void MLX90363::handleAlphaBeta() {
 alpha = RxBuffer[0] | (RxBuffer[1] & 0x3f) << 8;
 beta  = RxBuffer[2] | (RxBuffer[3] & 0x3f) << 8;
 
 err   = RxBuffer[1] >> 6;
 VG    = RxBuffer[4];
 ROLL  = RxBuffer[6] & 0x3f;
}

void MLX90363::handleXYZ() {
 X = RxBuffer[0] | (RxBuffer[1] & 0x3f) << 8;
 Y = RxBuffer[2] | (RxBuffer[3] & 0x3f) << 8;
 Z = RxBuffer[4] | (RxBuffer[5] & 0x3f) << 8;
 
 err = RxBuffer[1] >> 6;
 ROLL = RxBuffer[6] & 0x3f;
}

void MLX90363::prepareGET1Message(MessageType const type, const u2 timeout, bool const resetRoll) {
 if (isTransmitting()) return;
 TxBuffer[0] = 0;
 TxBuffer[1] = resetRoll;
 TxBuffer[2] = timeout & 0xff;
 TxBuffer[3] = timeout >> 8;
 TxBuffer[4] = 0;
 TxBuffer[5] = 0;
 TxBuffer[6] = (u1)type << 6 | (u1)Opcode::GET1;
 fillTxBufferCRC();
}

void MLX90363::startTransmitting(){
 if (isTransmitting()) return;
 startTransmittingUnsafe();
}
