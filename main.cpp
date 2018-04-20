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
#include "SerialInterface.h"

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
  ThreePhaseController::init(); // (comment out for digital cotrol tests)

  // Clear the MCU Status Register.  Indicates previous reset's source.
  MCUSR = 0;

  // Set up the hall sensor interrupts
  HallWatcher::init();

  // Set Enable Interrupts.
  sei();

  // holdUntilButtonFallingEdge();

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

  SerialInterface::init();

  while (1) {
    while (!SerialInterface::isMessageReady());
    SerialInterface::receiveMessage();
    auto msg = SerialInterface::getMessage();

    setPWM(msg->getCommand());
  }

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
