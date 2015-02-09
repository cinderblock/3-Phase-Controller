/* 
 * File:   I2C.h
 * Author: Cameron
 *
 * Created on January 12, 2015, 12:15 AM
 */

#ifndef I2C_H
#define	I2C_H

#include <avr/io.h>
#include "bitTypes.h"

namespace AVR {
 namespace I2C {
  
  typedef union {
   struct {
    /** @name TWIE
     * When this bit is written to one, and the I-bit in SREG is set, the TWI
     * interrupt request will be activated for as long as the TWINT Flag is high.
     */
    b1 InterruptEnable:1;
    u1:1;
    /** TWEN
     * The TWEN bit enables TWI operation and activates the TWI interface. When
     * TWEN is written to one, the TWI takes control over the I/O pins connected
     * to the SCL and SDA pins, enabling the slew-rate limiters and spike
     * filters. If this bit is written to zero, the TWI is switched off and all
     * TWI transmissions are terminated, regardless of any ongoing operation.
     */
    b1 Enable:1;
    /** TWWC
     * The TWWC bit is set when attempting to write to the TWI Data Register –
     * TWDR when TWINT is low. This flag is cleared by writing the TWDR Register
     * when TWINT is high.
     */
    b1 WriteCollisionFlag:1;
    /** TWSTO
     * Writing the TWSTO bit to one in Master mode will generate a STOP
     * condition on the 2-wire Serial Bus. When the STOP condition is executed
     * on the bus, the TWSTO bit is cleared automatically. In Slave mode,
     * setting the TWSTO bit can be used to recover from an error condition.
     * This will not generate a STOP condition, but the TWI returns to a
     * well-defined unaddressed Slave mode and releases the SCL and SDA lines to
     * a high impedance state.
     */
    b1 StopCondition:1;
    /** TWSTA
     * The application writes the TWSTA bit to one when it desires to become a
     * Master on the 2-wire Serial Bus. The TWI hardware checks if the bus is
     * available, and generates a START condition on the bus if it is free.
     * However, if the bus is not free, the TWI waits until a STOP condition is
     * detected, and then generates a new START condition to claim the bus
     * Master status. TWSTA must be cleared by software when the START condition
     * has been transmitted.
     */
    b1 StartCondition:1;
    /** TWEA
     * The TWEA bit controls the generation of the acknowledge pulse. If the
     * TWEA bit is written to one, the ACK pulse is generated on the TWI bus if
     * the following conditions are met:
     *  1. The device’s own slave address has been received.
     *  2. A general call has been received, while the TWGCE bit in the TWAR is set.
     *  3. A data byte has been received in Master Receiver or Slave Receiver mode.
     * By writing the TWEA bit to zero, the device can be virtually disconnected
     * from the 2-wire Serial Bus temporarily. Address recognition can then be
     * resumed by writing the TWEA bit to one again.
     */
    b1 EnableAcknowledge:1;
    /** TWINT
     * This bit is set by hardware when the TWI has finished its current job and
     * expects application software response. If the I-bit in SREG and TWIE in
     * TWCR are set, the MCU will jump to the TWI Interrupt Vector. While the
     * TWINT Flag is set, the SCL low period is stretched. The TWINT Flag must
     * be cleared by software by writing a logic one to it. Note that this flag
     * is not automatically cleared by hardware when executing the interrupt
     * routine. Also note that clearing this flag starts the operation of the
     * TWI, so all accesses to the TWI Address Register (TWAR), TWI Status
     * Register (TWSR), and TWI Data Register (TWDR) must be complete before
     * clearing this flag.
     */
    b1 InterruptFlag:1;
   };
   u1 byte;
  } CRt;

  typedef union {
   struct {
    b2 Prescaler:2;
    u1:1;
    b5 Status:5;
   };
   u1 byte;
  } SRt;

  typedef union {
   struct {
    b1 GeneralCallRecognition:1;
    b7 SlaveAddress:7;
   };
   u1 byte;
  } ARt;

  constexpr volatile u1  * const BR = &TWBR;

  /**
   * The TWCR is used to control the operation of the TWI. It is used to enable
   * the TWI, to initiate a Master access by applying a START condition to the
   * bus, to generate a Receiver acknowledge, to generate a stop condition, and
   * to control halting of the bus while the data to be written to the bus are
   * written to the TWDR. It also indicates a write collision if data is
   * attempted written to TWDR while the register is inaccessible.
   */
  constexpr volatile CRt * const CR  = (volatile CRt * const)&TWCR;
  constexpr volatile SRt * const SR  = (volatile SRt * const)&TWSR;
  constexpr volatile u1  * const DR  =                       &TWDR;
  constexpr volatile ARt * const AR  = (volatile ARt * const)&TWAR;
  constexpr volatile u1  * const AMR =                       &TWAMR;

  enum class Status : u1 {
   MA_STARTTransmitted = 0x08,
   MA_RepeatedSTARTTransmitted = 0x10,
   MA_SLA_W_ACK = 0x18,
   MA_SLA_W_NACK = 0x20,
   MA_DATA_TX_ACK = 0x28,
   MA_DATA_TX_NACK = 0x30,
   MA_ArbitrationLost = 0x38,

   MA_SLA_R_ACK = 0x40,
   MA_SLA_R_NACK = 0x48,
   MA_DATA_RX_ACK = 0x50,
   MA_DATA_RX_NACK = 0x58,
   
   SL_SLA_W_ACK = 0x60,
   SL_SLA_W_ACK_ArbitrationLost = 0x68,
   SL_General_ACK = 0x70,
   SL_General_ACK_ArbitrationLost = 0x78,
   SL_DATA_RX_ACK = 0x80,
   SL_DATA_RX_NACK = 0x88,
   SL_General_DATA_RX_ACK = 0x90,
   SL_General_DATA_RX_NACK = 0x98,
   SL_STOP_RESTART = 0xA0,
   
   SL_SLA_R_ACK = 0xA8,
   SL_SLA_R_ACK_ArbitrationLost = 0xB0,
   SL_DATA_TX_ACK = 0xB8,
   SL_DATA_TX_NACK = 0xC0,
   SL_DATA_TX_ACK_Done = 0xC8,
   
   NoState = 0xF8,
   BusError = 0x00
  };
  
  volatile inline Status getStatus() {
   return (Status)(SR->Status << 3);
  }
 };
};

#endif	/* I2C_H */

