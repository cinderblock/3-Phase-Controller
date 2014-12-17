/* 
 * File:   MLX90363.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:11 PM
 */

#ifndef MLX90363_H
#define	MLX90363_H

#include <avr/interrupt.h>

#include "basicTypes.h"

ISR(SPI_STC_vect);

class MLX90363 {
 friend void SPI_STC_vect();
 
 static const u1 messageLength = 8;
 
 static u1 buffer[messageLength];
 static u1 bufferPosition;
 
 static void startTransmitting();
 
public:
 static void init();
 inline static bool isTransmitting() {return bufferPosition < messageLength;}
 
 void sendMessage();
 

};

#endif	/* MLX90363_H */

