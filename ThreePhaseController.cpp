/* 
 * File:   ThreePhaseController.cpp
 * Author: Cameron
 * 
 * Created on October 22, 2015, 2:21 AM
 */

#include "ThreePhaseController.h"
#include "MLX90363.h"
#include "ThreePhaseDriver.h"

void ThreePhaseController::init() {
 MLX90363::init();
 ThreePhaseDriver::init();
}
