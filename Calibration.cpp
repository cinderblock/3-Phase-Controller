/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Calibration.cpp
 * Author: Cameron
 * 
 * Created on April 11, 2017, 6:49 PM
 */

#include <util/delay.h>

#include "Calibration.h"
#include "ThreePhaseDriver.h"
#include "MLX90363.h"
#include "Debug.h"
#include "HallWatcher.h"

using namespace ThreePhaseControllerNamespace;
using namespace Calibration;

void Calibration::main() {
  if (!enabled) return;
  
  ThreePhaseDriver::init();
  HallWatcher::init();
  MLX90363::init();
  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
  ThreePhaseDriver::setAmplitude(0);
  ThreePhaseDriver::setDeadTimes({15, 15});

  auto magRoll = MLX90363::getRoll();
  
  Board::LED::on();

  do {
    MLX90363::startTransmitting();

    // Hang while transmitting
    while (MLX90363::isTransmitting());

    // Delay long enough to guarantee data is ready
    _delay_ms(2);

    // Loop until we actually receive real data
  } while (!MLX90363::hasNewData(magRoll));

  // If numberOfSpins is too large, we should get a compile time overflow error
  constexpr u4 steps = u4(ThreePhaseDriver::StepsPerCycle) * numberOfSpins;

  constexpr u1 stepSize = 1;

  using namespace Debug;

  u4 i = 0;

  dout << PSTR("Calibration Mode");

  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;

  SOUT << Printer::Special::Start
      << u1(0)
      << u4(steps) << u1(amplitude)
      << Printer::Special::End;

  // Ramp up

  dout << PSTR("Forward ramp");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;

  for (; i < ThreePhaseDriver::StepsPerCycle; i += stepSize) {
    ThreePhaseDriver::setAmplitude(i * amplitude / rampSteps);
    step(i);
  }

  ThreePhaseDriver::setAmplitude(amplitude);

  dout << PSTR("Forward");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;
  
  for (i = 0; i < steps; i += stepSize) {
    step(i);
    // Send data via debug serial port
    SOUT
      << Printer::Special::Start
      << u1(1)
      << i << MLX90363::getAlpha() << HallWatcher::getState() << s1(1)
      << Printer::Special::End;
  }

  dout << PSTR("Forward tail");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;

  for (; i < steps + ThreePhaseDriver::StepsPerCycle; i += stepSize) {
    step(i);
  }

  // Reverse start

  dout << PSTR("Reverse ramp");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;

  for (; i > steps; i -= stepSize) {
    step(i);
  }

  dout << PSTR("Reverse");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;
  
  do {
    i -= stepSize;

    step(i);
    // Send data via debug serial port
    SOUT
      << Printer::Special::Start
      << u1(1)
      << i << MLX90363::getAlpha() << HallWatcher::getState() << s1(-1)
      << Printer::Special::End;
  } while (i);
  
  dout << PSTR("Done");
  // "Mark" start of binary data
  SOUT << Printer::Special::Marker;

  ThreePhaseDriver::setAmplitude(0);
  ThreePhaseDriver::advanceTo(0);

  // Don't continue
  while (1);
}


 inline libCameron::DecPrintFormatter& operator<< (libCameron::DecPrintFormatter& out, const MLX90363::ResponseState res) {
   switch (res) {
     case MLX90363::ResponseState::Init: return out << PSTR("Init");
     case MLX90363::ResponseState::Ready: return out << PSTR("Ready");
     case MLX90363::ResponseState::Receiving: return out << PSTR("Receiving");
     case MLX90363::ResponseState::Received: return out << PSTR("Received");
     case MLX90363::ResponseState::failedCRC: return out << PSTR("failedCRC");
     case MLX90363::ResponseState::TypeA: return out << PSTR("TypeA");
     case MLX90363::ResponseState::TypeAB: return out << PSTR("TypeAB");
     case MLX90363::ResponseState::TypeXYZ: return out << PSTR("TypeXYZ");
     case MLX90363::ResponseState::Other: return out << PSTR("Other");
     default: return out;
   }
 }

void Calibration::step(u4 i) {
  // Move to next position
  ThreePhaseDriver::advanceTo(i);

  // Give the motor some time to move
  _delay_ms(2);
  // Start the ADC sample on the MLX. We're going to throw away the data from this reading
  MLX90363::startTransmitting();

  // Hang while transmitting
  while (MLX90363::isTransmitting());

  while (1) {
    // Wait for a reading to be ready
    _delay_ms(2);
    // Record current roll value
    auto magRoll = MLX90363::getRoll();

    // Start SPI sequence
    MLX90363::startTransmitting();

    while (MLX90363::isTransmitting());

    // Hang while transmitting
    while (1) {
//      Debug::dout << PSTR("RES ") << MLX90363::getResponseState();
      Debug::SOUT << Debug::Printer::Special::Marker;
     switch (MLX90363::getResponseState()) {
       case MLX90363::ResponseState::Receiving:
       case MLX90363::ResponseState::Received:
         continue;
       case MLX90363::ResponseState::failedCRC:
       case MLX90363::ResponseState::Other:
       default:
         // Break and send message again
         break;
       case MLX90363::ResponseState::TypeA:
         return;
      }
     break;
    }
  }
}
