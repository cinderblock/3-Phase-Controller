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
  ThreePhaseDriver::init();
  MLX90363::init();
  MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);
  ThreePhaseDriver::setAmplitude(amplitude);
  ThreePhaseDriver::setDeadTimes({1, 1});

  auto magRoll = MLX90363::getRoll();

  do {
    MLX90363::startTransmitting();
    // Delay long enough to guarantee data is ready
    _delay_ms(2);

    // Loop until we actually receive real data
  } while (!MLX90363::hasNewData(magRoll));

  // If numberOfSpins is too large, we should get a compile time overflow error
  constexpr u2 steps = DriverConstants::StepsPerRotation * numberOfSpins;

  using namespace Debug;

  SOUT
      << Printer::Special::Start
      << steps
      << Printer::Special::End;
  
  for (u2 i = 0; i < steps; i++) {
    // Move to next position
    ThreePhaseDriver::advanceTo(i);

    // Give the motor some time to move
    _delay_ms(1);
    // Start the ADC sample on the MLX. We're going to throw away the data from this reading
    MLX90363::startTransmitting();
    
    // Wait for a reading to be ready
    _delay_ms(2);
    // Record current roll value
    magRoll = MLX90363::getRoll();
    
    // Start SPI sequence
    MLX90363::startTransmitting();
    
    // Wait for SPI sequence to finish
    // TODO: check in case crc fails and we'd be sitting here forever
    while (!MLX90363::hasNewData(magRoll));

    // Send data via debug serial port
    SOUT
        << Printer::Special::Start
        << i << MLX90363::getAlpha() << HallWatcher::getState()
        << Printer::Special::End;
  }

  ThreePhaseDriver::setAmplitude(0);
  ThreePhaseDriver::advanceTo(0);

  // Don't continue
  while (1);
}