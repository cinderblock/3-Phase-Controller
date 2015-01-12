/* 
 * File:   TwillBotInterface.cpp
 * Author: Cameron
 * 
 * Created on January 12, 2015, 12:33 AM
 */

#include "TwillBotInterface.h"
#include "I2C.h"

using namespace AVR::I2C;

void TwillBotInterface::init() {
 CR->byte = 0b01000101;

 *(ARt*const)AR = {true,address};
}
