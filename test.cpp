
#include "IOpin.h"
#include <avr/io.h>

IOpin test = IOpin(&PORTD, 0);

inline void ALowOff() {PORTB &= ~(1 << 4);}