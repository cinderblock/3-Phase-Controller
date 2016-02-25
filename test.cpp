
#include <AVR++/IOpin.h>
#include <avr/io.h>

// # HENRY: I removed "test" in the makefile, might want to add it back


template <unsigned int port> void setPort() {*(volatile uint8_t *)port = 0xff;}
//template <volatile uint8_t & port> void setPort() {port = 0xff;}

static volatile uint8_t foo;

static volatile uint8_t * const bar = &foo;

template <&PORTB> setPort();
//template <*(volatile uint8_t * const)0x05)> setPort();
//template <foo> setPort();
template <5> setPort();

