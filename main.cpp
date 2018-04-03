/**
 * File:   main.cpp
 * Author: btacklind
 *
 * Created on Nov 28, 2014, 2:02 PM
 */

#include "Board.h"
#include <AVR++/I2C.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "MLX90363.h"
#include "ThreePhaseController.h"
// #include "Debug.h"
#include "Calibration.h"
#include "Clock.h"
#include "Debug.h"
#include "Demo.h"
#include "HallWatcher.h"
#include "Interpreter.h"
#include "ServoController.h"
#include "commutation.h"

using namespace AVR;
using namespace ThreePhaseControllerNamespace;

#include "mainHelper.inc"

/**
 * All the init functions should go in here.
 * gcc will automatically call it for us because of the constructor attribute.
 */
void init() __attribute__((constructor));

u1 resetCause = MCUSR;

u1 initializeLIS331();

void init() {
  // Watch Dog Timer
  wdt_reset();
  wdt_disable();

  Debug::init();
  Debug::dout << PSTR("Beginning Inits \r\n");

  // Set up the driver pins in pwm mode
  ThreePhaseDriver::init(); // (comment out for digital cotrol tests)

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set up the hall sensor interrupts
  HallWatcher::init();

  // Set Enable Interrupts.
  sei();

  // holdUntilButtonFallingEdge();

  Debug::dout << PSTR("I");

  Debug::dout << initializeLIS331();

  Debug::dout << '\r' << '\n';

  Board::SPI::SCLK::off();
  Board::SPI::MISO::enablePullUp();
  Board::SPI::MOSI::off();

  MLX90363::init();

  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  auto magRoll = MLX90363::getRoll();

  Debug::dout << PSTR("Starting MLX readings\r\n");

  do {
    Board::LED1::on();
    Board::LED1::off();
    MLX90363::startTransmitting();
    // while (1);
    // Delay long enough to guarantee data is ready
    _delay_ms(3);
    // Loop until we actually receive real data
    break;
  } while (!MLX90363::hasNewData(magRoll));

  // Enable timer overflow interrupt to use for automatically reading MLX
  TIMSK4 = 1 << TOIE4;

  // Use the Clock that is outside the AVR++ namespace.
  ::Clock::init();

  Board::LED0::off(); // lED0 is used in debug routines.

  // End of init
  Debug::dout << PSTR("End of Inits \r\n");
}

constexpr u1 cyclesPWMPerMLX = 40;

void TIMER4_OVF_vect() {
  // Debug Trigger
  Board::LED1::on();
  Board::LED1::off();

  return;
  //
  u1 static mlx = cyclesPWMPerMLX;

  // Automatically start MLX communications every few ticks
  if (!--mlx) {
    MLX90363::startTransmitting();
    mlx = cyclesPWMPerMLX;
  }
}

Clock::MicroTime nextTime;

void printHallStateAndNumberIfHallChanged(u2 number) {
  u1 static lastState = HallWatcher::getState();

  const u1 state = HallWatcher::getState();

  if (state == lastState)
    return;

  lastState = state;
  Debug::dout << PSTR("New Hall State: ") << state << ',' << number << '\r'
              << '\n';
}

u1 sendI2CProgMemBlock(u1 const *block, u1 len) {
  while (len) {
    AVR::I2C::sendByte(pgm_read_byte(block++));
    while (!AVR::I2C::isFlagSet())
      ;

    if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterDataTransmittedAcked)
      return len;
    len--;
  }
  return 0;
}

u1 constexpr LIS311configLen = 5;
u1 const LIS311config[LIS311configLen] PROGMEM = {
    // Address byte with auto increment bit
    0x20 | (1 << 7),
    // CTRL_REG1 (20h) Normal mode, 1000Hz, Z enabled
    0b00111100,
    // CTRL_REG2 (21h) All defaults selected
    0b00000000,
    // CTRL_REG3 (22h) All defaults selected
    0b00000000,
    // CTRL_REG4 (23h) Don't update data between reads.  Set to +/- 12 g range.
    0b10010000,
};

// Return 0 on success. Otherwise failure.
u1 initializeLIS331() {
  // SCL Freq = F_CPU / (16 + 2 * BitRateRegister * 4 ^ Prescaler)
  // If F_CPU = 16Mhz, BitRateRegister = 18, & Prescaler = 1, then SCL Freq =
  // 100kHz
  AVR::I2C::setBitRateRegister(108);
  AVR::I2C::setPrescaler(1);

  AVR::I2C::CR->Enable = true;

  AVR::I2C::sendSTART();
  // Wait for START to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterStartTransmitted)
    return 10;

  // Write to I2C address 0x18
  AVR::I2C::sendByte(0x18 << 1);
  // Wait for it to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterWriteAcked)
    return 20;

  if (u1 ret = sendI2CProgMemBlock(LIS311config, LIS311configLen))
    return 30 + ret;

  AVR::I2C::sendSTART();
  // Wait for START to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterStartRepeatTransmitted)
    return 40;

  // Write to I2C address 0x19
  AVR::I2C::sendByte(0x19 << 1);
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterWriteAcked)
    return 50;

  if (u1 ret = sendI2CProgMemBlock(LIS311config, LIS311configLen))
    return 60 + ret;

  AVR::I2C::sendSTOP();

  // while (AVR::I2C::isStopping())
  Debug::dout << PSTR("Stopping\r\n");

  return 0;
}

u2 readLIS331Word(u1 const device, u1 const addr) {

  // Debug::dout << PSTR("I2C Start status: ") << (u1)AVR::I2C::getStatus() <<
  // '\r' << '\n';
  AVR::I2C::sendSTART();
  // Wait for START to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  // Debug::dout << PSTR("I2C START sent\r\n");
  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterStartTransmitted)
    return 0xfff8;

  // Write to I2C device
  AVR::I2C::sendByte((device << 1) | 0);
  // Wait for it to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  // Debug::dout << PSTR("I2C Device Addr sent\r\n");
  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterWriteAcked)
    return 0xfffe;

  // Read from device address with auto increment
  AVR::I2C::sendByte(addr | (1 << 7));
  // Wait for it to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterDataTransmittedAcked)
    return 0xfffd;

  AVR::I2C::sendSTART();
  // Wait for START to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterStartRepeatTransmitted)
    return 0xfffc;

  // Read from I2C device
  AVR::I2C::sendByte((device << 1) | 1);
  // Wait for it to actually send
  while (!AVR::I2C::isFlagSet())
    ;

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterReadAcked)
    return 0xfffb;

  // Prepare to receive a byte
  AVR::I2C::setupACK();

  while (!AVR::I2C::isFlagSet())
    ;
  // Wait for byte

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterDataReceivedAcked)
    return 0xfffa;

  u1 temp = AVR::I2C::getByte();

  // Prepare to receive a byte again
  AVR::I2C::setupNACK();

  while (!AVR::I2C::isFlagSet())
    ;
  // Wait for byte

  if (AVR::I2C::getStatus() != AVR::I2C::Status::MasterDataReceivedNacked)
    return 0xfff9;

  u1 temp2 = AVR::I2C::getByte();

  AVR::I2C::sendSTOP();
  // while (AVR::I2C::isStopping())
  // ;

  return temp << 8 | temp2;
}

u1 getSerialByte() {
  while (!(UCSR1A & 0b10000000))
    ;
  return UDR1;
}

u2 getSerialWord() {
  u1 first = getSerialByte();
  u1 second = getSerialByte();

  return first << 8 | second;
}

void doState(bool al, bool bl, bool cl, bool ah, bool bh, bool ch) {

  // Board::DRV::AL::set(0); //  all off for a moment
  // Board::DRV::BL::set(0);
  // Board::DRV::CL::set(0);
  // Board::DRV::AH::set(0);
  // Board::DRV::BH::set(0);
  // Board::DRV::CH::set(0);
  // _delay_ms(20);

  Board::DRV::AL::set(al);
  Board::DRV::BL::set(bl);
  Board::DRV::CL::set(cl);
  Board::DRV::AH::set(ah);
  Board::DRV::BH::set(bh);
  Board::DRV::CH::set(ch);
  _delay_ms(20);
}

/**
 *
 **/
int main() {

  // while (1) {
  //
  //   //     al,bl,cl,ah,bh,ch
  //   doState(1, 0, 0, 0, 1, 0); // 6
  //   doState(0, 0, 1, 0, 1, 0); // 4
  //   doState(0, 0, 1, 1, 0, 0); // 5
  //   doState(0, 1, 0, 1, 0, 0); // 1
  //   doState(0, 1, 0, 0, 0, 1); // 3
  //   doState(1, 0, 0, 0, 0, 1); // 2

  // Set to 2 volts with 1A for this test
  // doState(1, 0, 0, 1, 0, 0); // short A
  // doState(0, 1, 0, 0, 1, 0); // short B
  // doState(0, 0, 1, 0, 0, 1); // short C

  // short all
  // doState(1, 1, 1, 1, 1, 1); // short All
  // }

  // Clock::MicroTime next;
  //
  // Clock::readTime(next);
  //
  // while (1) {
  //
  //   Board::LED_LIS::on();
  //   Board::LED_LIS::off();
  //
  //   next += 10_ms;
  //
  //   while (!next.isInPast());
  //
  //   Debug::dout << readLIS331Word(0x18, 0x2C) << '\r' << '\n';
  //
  //   // while (1);
  // }

  // setPWM(60);

  // get a byte from the master and set the pwm
  u1 i = 0;

  // setPWM(-100);
  // while (1)
  //   ;

  while (1) {
    while (getSerialByte() != 0xff)
      ;

    u2 command = getSerialWord();
    if (command != getSerialWord())
      continue;

    setPWM((s2)command);
    // setPWM(-100);

    // Respond with a counter
    UDR1 = i++;
  };

  while (0) {

    printHallStateIfChanged();
    HallWatcher::checkAndUpdate();

    Debug::dout << PSTR("Holding for button press \r\n");
    holdUntilButtonFallingEdge();

    doSinePulse(255);
  }

  // loop in case main loop is disabled
  // allows for interrupts to continue
  while (1)
    ;
};
