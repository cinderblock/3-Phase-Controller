/* 
 * File:   MLX90363.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:11 PM
 */

#ifndef MLX90363_H
#define	MLX90363_H

#include <avr/interrupt.h>

#include <AVR++/bitTypes.h>
#include <AVR++/IOpin.h>
#include "Board.h"

ISR(SPI_STC_vect);

class MLX90363 {
 
 /**
  * The fixed message length that the MLX90363 sends
  */
 static constexpr u1 messageLength = 8;
 
 /**
  * Staged transmit buffer. Will be sent automatically by the interrupt routine
  * if properly started and left alone so that repeat messages are trivial.
  */
 static u1 TxBuffer[messageLength];
 
 /**
  * The buffer for the incoming/received message
  */
 static u1 RxBuffer[messageLength];
 
 /**
  * Where we are while sending/reading the data in the SPI interrupt
  */
 static u1 bufferPosition;
 
 enum class ResponseState : u1;
 
 static ResponseState responseState;
 
 static u2 alpha;
 static u2 beta;
 static u2 X;
 static u2 Y;
 static u2 Z;
 static u1 err;
 static u1 VG;
 static u1 ROLL;
 
 /**
  * INTERNAL: Reset the buffer position and start the transmission sequence.
  * 
  * Unsafe because it does not check if there is already a transmission running.
  */
 static void startTransmittingUnsafe();
 
 /**
  * Calculate and write the correct CRC for the message in and to the TxBuffer
  */
 static void fillTxBufferCRC();
 
 /**
  * Check the checksum of the data in the RxBuffer
  * @return 
  */
 static bool checkRxBufferCRC();
 
 /**
  * The slave select line
  */
 static constexpr IOpin &SS = Board::MagSel;
 static constexpr IOpin &MISO = Board::SPI::MISO;
 static constexpr IOpin &MOSI = Board::SPI::MOSI;
 static constexpr IOpin &SCLK = Board::SPI::SCLK;
 
 /**
  * OpCodes from the MLX90363 datasheet.
  * 
  */
 enum class Opcode: b6 {
  // Following the format from the datasheet, they organized all the opcodes as
  // Outgoing      or       Incoming
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
 
 /**
  * Handle a standard Alpha response from the MLX90363
  */
 static void handleAlpha();
 
 /**
  * Handle a standard AlphaBeta response from the MLX90363
  */
 static void handleAlphaBeta();
 
 /**
  * Handle a standard XYZ response from the MLX90363
  */
 static void handleXYZ();
 
public:
 enum class ResponseState: u1 {
  Init, Ready, Receiving, Received, failedCRC, TypeA, TypeAB, TypeXYZ, Other
 };
 
 /**
  * The 2-bit marker attached to all incoming messages for easy processing
  */
 enum class MessageType: b2 {
  Alpha = 0, AlphaBeta = 1, XYZ = 2, Other = 3
 };
 
 /**
  * Initialize the hardware.
  * 
  * One thing that is not done is to set the AVR's hardware SS line to an output,
  * preventing the hardware from automatically switching to slave mode. Make sure
  * that it is set to an output (and stays that way) before calling this function.
  */
 static void init();
 
 static inline void isr();
 
 /**
  * Set the SPI hardware's divider
  * @param 
  */
 static void setSPISpeed(u1 const);

 /**
  * Check if we're still talking on the SPI bus
  * @return 
  */
 inline static bool isTransmitting() {
  // Any of these would work. Not sure which is most effective
//  return bufferPosition != messageLength;
  //return !SS.isHigh();
  // Since IOpin isn't optimized, lets use the low level test
//  return !(PORTD & (1<<5));
 }
 
 /**
  * Start sending whatever is in the buffer, unless it's already being sent
  * 
  */
 static void startTransmitting();
 
 static b6 getReceivedOpCode();

 /**
  * Handle a received message.
  * 
  * Checks the CRC
  * Checks for standard marker
  *  and passes the message off to another internal handler
  */
 static void handleResponse();
 
 inline static u2 getAlpha() {cli(); u2 const a = alpha; sei(); return a;}
 
 inline static u2 getBeta() {return beta;}
 
 inline static u2 getX() {return X;}
 
 inline static u2 getY() {return Y;}
 
 inline static u2 getZ() {return Z;}
 
 inline static u1 getRoll() {return ROLL;}
 
 inline static u1 getErr() {return err;}
 
 /**
  * Construct a standard GET1 message
  * @param type
  * @param timeout
  * @param resetRoll
  */
 static void prepareGET1Message(MessageType const type, const u2 timeout = 0xffff, bool const resetRoll = false);

};

#endif	/* MLX90363_H */

