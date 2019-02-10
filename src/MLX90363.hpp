/*
 * File:   MLX90363.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:11 PM
 */

#ifndef MLX90363_H
#define MLX90363_H

#include <avr/interrupt.h>

#include "Board.hpp"
#include "Clock.hpp"
#include <AVR++/Atomic.hpp>
#include <AVR++/IOpin.hpp>
#include <AVR++/bitTypes.hpp>

// Declare interrupt. Doesn't need to block others.
ISR(SPI_STC_vect, ISR_NOBLOCK);

namespace ThreePhaseControllerNamespace {

using namespace Basic;

class MLX90363 {
  friend void ::SPI_STC_vect();
  static inline void isr();

  static void (*volatile alphaHandler)(u2 const alpha);

  /**
   * The fixed message length that the MLX90363 sends
   */
  static constexpr u1 messageLength = 8;

  /**
   * Staged transmit buffer. Will be sent automatically by the interrupt routine.
   * If properly started and left alone, repeat messages are trivial.
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

public:
  enum class ResponseState : u1;

private:
  static ResponseState responseState;

  static volatile Atomic<u2> alpha;
  static volatile Atomic<u2> beta;
  static volatile Atomic<u2> X;
  static volatile Atomic<u2> Y;
  static volatile Atomic<u2> Z;
  static volatile u1 err;
  static volatile u1 VG;
  static volatile u1 ROLL;

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
   * OpCodes from the MLX90363 datasheet.
   *
   */
  enum class Opcode : b6 {
    // Following the format from the datasheet, they organized all the opcodes as
    // Outgoing      or       Incoming
    GET1 = 0x13,
    GET2 = 0x14,
    GET3 = 0x15,
    Get3Ready = 0x2D,
    MemoryRead = 0x01,
    MemoryRead_Answer = 0x02,
    EEPROMWrite = 0x03,
    EEPROMWrite_Challenge = 0x04,
    EEChallengeAns = 0x05,
    EEReadAnswer = 0x28,
    EEReadChallenge = 0x0F,
    EEPROMWrite_Status = 0x0E,
    NOP__Challenge = 0x10,
    Challenge__NOP_MISO_Packet = 0x11,
    DiagnosticDetails = 0x16,
    Diagnostics_Answer = 0x17,
    OscCounterStart = 0x18,
    OscCounterStart_Acknowledge = 0x19,
    OscCounterStop = 0x1A,
    OscCounterStopAck_CounterValue = 0x1B,
    Reboot = 0x2F,
    Standby = 0x31,
    StandbyAck = 0x32,
    Error_frame = 0x3D,
    NothingToTransmit = 0x3E,
    Ready_Message = 0x2C,
  };

#ifndef MLX90363_3V3_TIMINGS
  // 5V Timings (default)

  static constexpr auto tStartUp = 20_ms; // min

  static constexpr auto tShort = 120_us; // min

  static constexpr auto teewrite = 32_ms; // min

  static constexpr auto tDiag_1000Hz = 40_ms; // max
  static constexpr auto tDiag_500Hz = 20_ms;  // max
  static constexpr auto tDiag_200Hz = 10_ms;  // max

  static constexpr auto t1us = 1_us; // typ

  static constexpr auto tREFE_mod1_Alpha = 920_us;      // min
  static constexpr auto tREFE_mod1_AlphaBeta = 1050_us; // min
  static constexpr auto tREFE_mod1_XYZ = 920_us;        // min

  static constexpr auto tReady_mod1_Alpha = 920_us;      // max
  static constexpr auto tReady_mod1_AlphaBeta = 1050_us; // max
  static constexpr auto tReady_mod1_XYZ = 920_us;        // max

  static constexpr auto tSyncFE_Alpha = 874_us;      // min
  static constexpr auto tSyncFE_AlphaBeta = 1004_us; // min
  static constexpr auto tSyncFE_XYZ = 874_us;        // min

  static constexpr auto tReady_mod2_Alpha = 874_us;      // max
  static constexpr auto tReady_mod2_AlphaBeta = 1004_us; // max
  static constexpr auto tReady_mod2_XYZ = 874_us;        // max

  static constexpr auto tRESync = 80_us; // min

  static constexpr auto tRERE_mod3_Alpha = 950_us;      // min
  static constexpr auto tRERE_mod3_AlphaBeta = 1080_us; // min
  static constexpr auto tRERE_mod3_XYZ = 950_us;        // min

  static constexpr auto tReadyRE_mod3_Alpha = 950_us;      // max
  static constexpr auto tReadyRE_mod3_AlphaBeta = 1080_us; // max
  static constexpr auto tReadyRE_mod3_XYZ = 950_us;        // max

  static constexpr auto tREFE_mod3 = 90_us; // min

  static constexpr auto tReadyFE_mod3 = 90_us; // min
#else
  // 3V3 Timings

  static constexpr auto tStartUp = 23.2_ms; // min

  static constexpr auto tShort = 139_us; // min

  static constexpr auto teewrite = 37_ms; // min

  static constexpr auto tDiag_1000Hz = 46.4_ms; // max
  static constexpr auto tDiag_500Hz = 23.2_ms;  // max
  static constexpr auto tDiag_200Hz = 11.6_ms;  // max

  static constexpr auto t1us = 1_us; // typ

  static constexpr auto tREFE_mod1_Alpha = 1067_us;     // min
  static constexpr auto tREFE_mod1_AlphaBeta = 1218_us; // min
  static constexpr auto tREFE_mod1_XYZ = 1067_us;       // min

  static constexpr auto tReady_mod1_Alpha = 1067_us;     // max
  static constexpr auto tReady_mod1_AlphaBeta = 1218_us; // max
  static constexpr auto tReady_mod1_XYZ = 1067_us;       // max

  static constexpr auto tSyncFE_Alpha = 1014_us;     // min
  static constexpr auto tSyncFE_AlphaBeta = 1165_us; // min
  static constexpr auto tSyncFE_XYZ = 1014_us;       // min

  static constexpr auto tReady_mod2_Alpha = 1014_us;     // max
  static constexpr auto tReady_mod2_AlphaBeta = 1165_us; // max
  static constexpr auto tReady_mod2_XYZ = 1014_us;       // max

  static constexpr auto tRESync = 93_us; // min

  static constexpr auto tRERE_mod3_Alpha = 1102_us;     // min
  static constexpr auto tRERE_mod3_AlphaBeta = 1253_us; // min
  static constexpr auto tRERE_mod3_XYZ = 1102_us;       // min

  static constexpr auto tReadyRE_mod3_Alpha = 1102_us;     // max
  static constexpr auto tReadyRE_mod3_AlphaBeta = 1253_us; // max
  static constexpr auto tReadyRE_mod3_XYZ = 1102_us;       // max

  static constexpr auto tREFE_mod3 = 105_us; // min

  static constexpr auto tReadyFE_mod3 = 105_us; // min
#endif

  static constexpr auto tOscCounter_min = 500_us;
  static constexpr auto tOscCounter_typ = 1000_us;
  static constexpr auto tOscCounter_max = 30000_us;

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
  enum class ResponseState : u1 { Init, Ready, Receiving, Received, failedCRC, TypeA, TypeAB, TypeXYZ, Other };

  /**
   * The 2-bit marker attached to all messages for easy processing
   */
  enum class MessageType : b2 { Alpha = 0, AlphaBeta = 1, XYZ = 2, Other = 3 };

  /**
   * Initialize the hardware.
   *
   * One thing that is not done is to set the AVR's hardware SS line to an output,
   * preventing the hardware from automatically switching to slave mode. Make sure
   * that it is set to an output (and stays that way) before calling this function.
   */
  static void init();

  static inline bool hasNewData(u1 &lastRoll) {
    u1 const r = ROLL;
    if (r == lastRoll)
      return false;
    lastRoll = r;
    return true;
  }

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
    // return bufferPosition != messageLength;
    // return !SS.isHigh();
    // Let's use the low level test from Board::
    return Board::SPI::isSlaveSelected();
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

  // inline static u2 getAlpha() {return (u2 const) alpha;}
  inline static u2 getAlpha() { return alpha; }

  inline static u2 getBeta() { return beta; }

  inline static u2 getX() { return X; }

  inline static u2 getY() { return Y; }

  inline static u2 getZ() { return Z; }

  inline static u2 getAlphaUnsafe() { return alpha.getUnsafe(); }

  inline static u2 getBetaUnsafe() { return beta.getUnsafe(); }

  inline static u2 getXUnsafe() { return X.getUnsafe(); }

  inline static u2 getYUnsafe() { return Y.getUnsafe(); }

  inline static u2 getZUnsafe() { return Z.getUnsafe(); }

  // Slightly more efficient versions of the safe ones above

  inline static u2 getAlphaForceInterruptsOn() { return alpha.getForceInterruptsOn(); }

  inline static u2 getBetaForceInterruptsOn() { return beta.getForceInterruptsOn(); }

  inline static u2 getXForceInterruptsOn() { return X.getForceInterruptsOn(); }

  inline static u2 getYForceInterruptsOn() { return Y.getForceInterruptsOn(); }

  inline static u2 getZForceInterruptsOn() { return Z.getForceInterruptsOn(); }

  inline static u1 getRoll() { return ROLL; }

  inline static u1 getErr() { return err; }

  /**
   * Construct a standard GET1 message
   * @param type
   * @param timeout
   * @param resetRoll
   */
  static void prepareGET1Message(MessageType const type, const u2 timeout = 0xffff, bool const resetRoll = false);

  static void prepareReadMessage(u2 const addr0, u2 const addr1);

  static constexpr u1 resolutionBits = 14;

  static inline void setAlphaHandler(void (*handler)(u2 const)) { alphaHandler = handler; }

  static inline ResponseState getResponseState() { return responseState; }

  static inline u1 const *const getTxBuffer() { return TxBuffer; }
  static inline u1 const *const getRxBuffer() { return RxBuffer; }

private:
  static inline void setCommandUnsafe(MessageType t, Opcode c) { TxBuffer[6] = (((u1)t) << 6) | (u1)c; }
  static inline void setCommandUnsafe(Opcode c) { setCommandUnsafe(MessageType::Other, c); }
};

}; // namespace ThreePhaseControllerNamespace

#endif /* MLX90363_H */
