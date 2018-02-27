/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MotorPosition.h
 * Author: Cameron
 *
 * Created on June 8, 2017, 2:31 PM
 */

#ifndef MOTORPOSITION_H
#define MOTORPOSITION_H

#include <AVR++/basicTypes.h>

#include "ThreePhaseDriver.h"

namespace ThreePhaseControllerNamespace {

  using namespace AVR;

  class MotorPosition {
    /**
     * Two bytes that represent the motor's angle.
     *
     * The lower 10-bits represent a ThreePhaseDriver::PhasePosition.
     * The upper 4-bits represent the 7 motor cycles per electrical revolution.
     */
    u2 position;
  public:
    inline MotorPosition(u2 pos): position(pos) {}

    /**
     * Gets the position normalized without any gaps
     * @return
     */
    inline u2 getMechanicalPosition() const {
      return (position & DriverConstants::MaskForPhase) + (position >> 12) * DriverConstants::StepsPerCycle;
    }

    /**
     * Gets the electrical phase represented by this MotorPosition
     * @return
     */
    inline ThreePhaseDriver::PhasePosition getPhasePosition() const {
      return position & DriverConstants::MaskForPhase;
    }
  };
}

#endif /* MOTORPOSITION_H */

