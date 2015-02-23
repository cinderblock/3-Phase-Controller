/* 
 * File:   TripleBuffer.h
 * Author: Cameron
 *
 * Created on February 22, 2015, 5:58 PM
 */

#ifndef TRIPLEBUFFER_H
#define	TRIPLEBUFFER_H

#include "basicTypes.h"

template <u1 size>
class TripleBuffer {
 u1 rBuffer[size];
 u1 gBuffer[size];
 u1 bBuffer[size];
 
 enum class State: u1 {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P};
 
 State state;
 
public:
 TripleBuffer();
 TripleBuffer(const TripleBuffer& orig);
 virtual ~TripleBuffer();
 
 u1 * getReadBuffer();
 
 u1 * getWriteBuffer();

};

#endif	/* TRIPLEBUFFER_H */

