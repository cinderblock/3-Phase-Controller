/* 
 * File:   Board.h
 * Author: bztacklind
 *
 * Created on March 25, 2014, 12:46 PM
 */

#ifndef BOARD_H
#define	BOARD_H

#include "IOpin.h"

//using namespace AVR;

class Board {
public:
    //For PWM control
    static IOpin AL;
    static IOpin SCLK;
    static IOpin MOSI;
    static IOpin MISO;
    static IOpin CL;
    static IOpin CH;
    static IOpin BH;
    static IOpin AH;
    static IOpin SCL;
    static IOpin SDA;
    static IOpin Rx;
    static IOpin Tx;
    static IOpin LED;
    static IOpin MagSel;
    static IOpin AS;
    static IOpin CS;
    static IOpin AIN0;
    static IOpin BL;
    static IOpin BS;
    static IOpin VBATS;
    
private:

};

#endif	/* BOARD_H */