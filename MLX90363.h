/* 
 * File:   MLX90363.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:11 PM
 */

#ifndef MLX90363_H
#define	MLX90363_H

#include <avr/interrupt.h>

#include "bitTypes.h"
#include "IOpin.h"
#include "Board.h"

ISR(SPI_STC_vect);

class MLX90363 {
 friend void SPI_STC_vect();
 
 static const u1 messageLength = 8;
 
 static u1 buffer[messageLength];
 static u1 bufferPosition;
 
 static void startTransmitting();
 
 static u1 getBufferCRC();
 inline static void fillBufferCRC() {buffer[7] = getBufferCRC();}
 inline static bool checkBufferCRC() {return buffer[7] == getBufferCRC();}
 
 static constexpr IOpin &SS = Board::MagSel;
 
 enum class Opcode: b6 {
  GET1 = 0x13,
  GET2 = 0x14,
  GET3 = 0x15,              Get3Ready = 0x2D,
  MemoryRead = 0x01,        MemoryRead_Answer = 0x02,
  EEPROMWrite = 0x03,       EEPROMWrite_Challenge = 0x04,
  EEChallengeAns = 0x05,    EEReadAnswer = 0x28,
  EEReadChallenge = 0x0F,   EEPROMWrite_Status = 0x0E,
  NOP__Challenge = 0x10,    Challenge__NOP_MISO_Packet = 0x11,
  DiagnosticDetails = 0x16, Diagnostics_Answer = 0x17,
  OscCounterStart = 0x18,   OscCounterStart_Acknowledge = 0x19,
  OscCounterStop = 0x1A,    OscCounterStopAck_CounterValue = 0x1B,
  Reboot = 0x2F,
  Standby = 0x31,           StandbyAck = 0x32,
                            Error_frame = 0x3D,
                            NothingToTransmit = 0x3E,
                            Ready_Message = 0x2C,
 };
 
public:
 static void init();
 static void setSPISpeed(u1 const);

 inline static bool isTransmitting() {return bufferPosition < messageLength;}

 void sendMessage();
 

};

#endif	/* MLX90363_H */

