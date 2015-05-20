/* 
 * File:   TripleBuffer.h
 * Author: Cameron
 *
 * Created on February 22, 2015, 5:58 PM
 */

#ifndef TRIPLEBUFFER_H
#define	TRIPLEBUFFER_H

#include <AVR++/basicTypes.h>

using namespace AVR;

template <u1 size, bool readInterrupt, bool writeInterrupt = !readInterrupt>
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
  * Reserve the newest block for reading
  * @return 
  */
 void reserveNewestBufferForReading();
 
 /**
  * Mark the buffer the writer is using as the newest, indicating that data is
  * available and allowing the reader to access the next block
  * @return 
  */
 void markNewestBuffer();
 
 /**
  * Check if there is new data available for the reader side
  * @return 
  */
 bool isNewData();
 
 /**
  * Get the buffer that should be written to
  * @return 
  */
 u1 * getWriteBuffer();
 
 /**
  * Get the buffer that should be read from
  * @return 
  */
 u1 * getReadBuffer();

};

#endif	/* TRIPLEBUFFER_H */

