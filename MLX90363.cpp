/* 
 * File:   MLX90363.cpp
 * Author: Cameron
 * 
 * Created on December 10, 2014, 4:11 PM
 */

#include "MLX90363.h"
#include "SPI.h"
#include "Board.h"

using namespace AVR::SPI;

void SPI_STC_vect() {
 if (MLX90363::bufferPosition + 1 != MLX90363::messageLength)
  *DR = MLX90363::buffer[MLX90363::bufferPosition + 1];
 MLX90363::buffer[MLX90363::bufferPosition++] = *DR;
}

u1 MLX90363::buffer[messageLength];
u1 MLX90363::bufferPosition = messageLength;

void MLX90363::init() {
 Board::MagSel.on();
 Board::MagSel.output();
 SR->byte = 0;
 CR->byte = 0b11010100;
 
 buffer[0] =
 buffer[1] =
 buffer[2] =
 buffer[3] =
 buffer[4] =
 buffer[5] =
 buffer[6] =
 buffer[7] = 0;
}

void MLX90363::startTransmitting() {
 if (isTransmitting()) return;
 bufferPosition = 0;
 *DR = buffer[bufferPosition];
}
