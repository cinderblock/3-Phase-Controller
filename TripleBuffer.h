/* 
 * File:   TripleBuffer.h
 * Author: Cameron
 *
 * Created on February 22, 2015, 5:58 PM
 */

#ifndef TRIPLEBUFFER_H
#define	TRIPLEBUFFER_H

#include "basicTypes.h"

template <u1 size, bool readInterrupt>
class TripleBuffer {
 
 // The three buffers of the Triple Buffer
 
 /**
  * The RED buffer
  */
 u1 rBuffer[size];
 
 /**
  * The GREEN buffer
  */
 u1 gBuffer[size];
 
 /**
  * The BLUE buffer
  */
 u1 bBuffer[size];
 
 /**
  * All possible buffer states.
  * 
  * @See Triple Buffer state machine diagram
  */
 enum class State: u1 {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P};
 
 /**
  * The 
  */
 State state;
 
public:
 TripleBuffer();
 
 /**
  * Get the next buffer for reading
  * @return 
  */
 u1 * getReadBuffer();
 
 /**
  * Get the next buffer for writing and let the reader read
  * @return 
  */
 u1 * getWriteBuffer();
 
 /**
  * Check if there is new data available for the reader side
  * @return 
  */
 bool isNewData();

};

#endif	/* TRIPLEBUFFER_H */

