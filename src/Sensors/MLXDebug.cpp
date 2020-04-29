/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MLXDebug.cpp
 * Author: Cameron
 *
 * Created on February 25, 2018, 10:27 PM
 */

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "../Debug.hpp"
#include "MLX90363.hpp"
#include "MLXDebug.hpp"
#include <AVR++/USART.hpp>

using namespace ThreePhaseControllerNamespace;
using namespace AVR;
using namespace libCameron;

inline DecPrintFormatter &operator<<(DecPrintFormatter &out, const MLX90363::ResponseState rs) {
  auto prefix = PSTR("MLX:RS:");
  switch (rs) {
  case MLX90363::ResponseState::Init:
    return out << prefix << PSTR("Init");
  case MLX90363::ResponseState::Other:
    return out << prefix << PSTR("Other");
  case MLX90363::ResponseState::Ready:
    return out << prefix << PSTR("Ready");
  case MLX90363::ResponseState::Received:
    return out << prefix << PSTR("Received");
  case MLX90363::ResponseState::Receiving:
    return out << prefix << PSTR("Receiving");
  case MLX90363::ResponseState::TypeA:
    return out << prefix << PSTR("TypeA");
  case MLX90363::ResponseState::TypeAB:
    return out << prefix << PSTR("TypeAB");
  case MLX90363::ResponseState::TypeXYZ:
    return out << prefix << PSTR("TypeXYZ");
  case MLX90363::ResponseState::failedCRC:
    return out << prefix << PSTR("failedCRC");
  }
  return out;
}

char nibToHexChar(u1 const nib) {
  if (nib < 10)
    return '0' + nib;
  if (nib < 16)
    return 'A' - 10 + nib;
  return '*';
}

void printNibble(u1 nib) { Debug::dout << nibToHexChar(nib); }

void printHex(u1 byte) {
  printNibble(byte >> 4);
  printNibble(byte & 15);
}

void printMLXBuffer(u1 const *buff) {
  u1 length = 8;
  while (length--)
    printHex(*buff++);
}

static u1 checkRx() {
  if (UCSR1A & (1 << RXC1)) {
    return UDR1 = UDR1;
  }
  return 0;
}

void MLXDebug::main() {
  if (!enabled)
    return;

  Debug::dout << PSTR("MLX Debug\r\n");

  MLX90363::init();

  Debug::dout << PSTR("MLX90363::init() done\r\n");

  if (MLX90363::isTransmitting()) {
    Debug::dout << PSTR("Already Transmitting!\r\n");
  }

  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);

  Debug::dout << PSTR("Get 1 Message Ready\r\n");

  auto magRoll = MLX90363::getRoll();

  //  Board::LED::on();

  u1 i = 0;

  u1 repeat = 0;

  do {

    Debug::dout << PSTR("Tx: ");
    printMLXBuffer(MLX90363::getTxBuffer());
    Debug::dout << '\r' << '\n';

    auto state = MLX90363::getResponseState();

    Debug::dout << PSTR("MLX State: ") << state << '\r' << '\n';

    MLX90363::startTransmitting();
    // Wait for packet (get1 message) to actually send
    while (MLX90363::isTransmitting()) {
      auto newState = MLX90363::getResponseState();
      if (state != newState) {
        state = newState;
        Debug::dout << PSTR("New State: ") << state << '\r' << '\n';
      }
    }

    Debug::dout << PSTR("Rx: ");
    printMLXBuffer(MLX90363::getRxBuffer());
    Debug::dout << '\r' << '\n';

    Debug::dout << PSTR("State: ") << MLX90363::getResponseState();

    if (MLX90363::getResponseState() == MLX90363::ResponseState::TypeA) {
      Debug::dout << ' ' << MLX90363::getAlpha();
    }
    Debug::dout << '\r' << '\n';

    _delay_ms(2);

    if (repeat) {
      if (repeat != 0xff)
        repeat--;
      continue;
    }

    if (i == 5) {
      u1 r;
      while (!(r = checkRx()))
        ;
      if (r == '2')
        repeat = 1;
      if (r == ' ')
        repeat = 0xff;
    } else {
      i++;
    }

    // Loop until we actually receive real data
  } while (1);

  while (1)
    ;
}
