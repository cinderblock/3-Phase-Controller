#include <avr/io.h>
#include <stdint.h>
constexpr volatile uint8_t *const PortA = &PORTB;
int main() { *PortA |= 1; }

// constexpr auto PortA = &PORTB;

// int main() { *PortA |= 1; }

constexpr uintptr_t PA = 0x25;

void set() { (*(volatile uint8_t *)(PA)) |= 1; }

const char str[] = "FooBar";
constexpr const char *x = str + 2;