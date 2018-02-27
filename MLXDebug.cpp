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

#include <util/delay.h>

#include "MLXDebug.h"
#include "MLX90363.h"
#include "Debug.h"
#include <AVR++/USART.h>

using namespace ThreePhaseControllerNamespace;
using namespace AVR;
using namespace libCameron;

inline DecPrintFormatter& operator<< (DecPrintFormatter& out, const MLX90363::ResponseState rs) {
  auto prefix = PSTR("MLX:RS:");
  switch (rs) {
    case MLX90363::ResponseState::Init: return out << prefix << PSTR("Init");
    case MLX90363::ResponseState::Other: return out << prefix << PSTR("Other");
    case MLX90363::ResponseState::Ready: return out << prefix << PSTR("Ready");
    case MLX90363::ResponseState::Received: return out << prefix << PSTR("Received");
    case MLX90363::ResponseState::Receiving: return out << prefix << PSTR("Receiving");
    case MLX90363::ResponseState::TypeA: return out << prefix << PSTR("TypeA");
    case MLX90363::ResponseState::TypeAB: return out << prefix << PSTR("TypeAB");
    case MLX90363::ResponseState::TypeXYZ: return out << prefix << PSTR("TypeXYZ");
    case MLX90363::ResponseState::failedCRC: return out << prefix << PSTR("failedCRC");
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

void printNibble(u1 nib) {
  Debug::dout << nibToHexChar(nib);
}

void printHex(u1 byte) {
  printNibble(byte >> 4);
  printNibble(byte & 15);
}

void printMLXBuffer(u1 const * buff) {
  u1 length = 8;
  while (length--) printHex(*buff++);
}


void MLXDebug::main() {
  if (!enabled) return;
  
  MLX90363::init();
  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
  
  auto magRoll = MLX90363::getRoll();
  
  Board::LED::on();

  do {
    
    Debug::dout << PSTR("Tx: ");
    printMLXBuffer(MLX90363::getTxBuffer());
    Debug::dout << '\r' << '\n';
    
    MLX90363::startTransmitting();
    // Wait for packet (get1 message) to actually send
    while (MLX90363::isTransmitting());
    
    Debug::dout << PSTR("Rx: ");
    printMLXBuffer(MLX90363::getRxBuffer());
    Debug::dout << '\r' << '\n';
    
    Debug::dout << PSTR("State: ") << MLX90363::getResponseState() << '\r' << '\n';
    
    // Just get a byte, any byte for now
    usart.get();
    
    // Always loop for now.
    continue;

    // Loop until we actually receive real data
  } while (!MLX90363::hasNewData(magRoll));
  
  
  while (1);
}

