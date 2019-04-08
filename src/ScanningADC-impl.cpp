#include "Board.hpp"
#include "main.hpp"
#include <AVR++/ScanningADC.cpp>

#ifdef HOVER_DRIVE

using namespace ThreePhaseControllerNamespace;

static constexpr u1 numberOfScannedAnalogInputs = 9;

// Atmega32u4 Datasheet Table 24-4
constexpr u1 TemperatureSensorMux = 0b100111;

template <u1 N>
typename ScanningADC<N>::Input ScanningADC<N>::inputs[] = {
    {{Board::MUX::AS, ADC::Reference::Internal}, &Analog::AS},
    {{Board::MUX::BS, ADC::Reference::Internal}, &Analog::BS},
    {{Board::MUX::CS, ADC::Reference::Internal}, &Analog::CS},
    {{Board::MUX::VBATS, ADC::Reference::Internal}, &Analog::battery},
    {{Board::MUX::VDDS, ADC::Reference::Internal}, &Analog::drive},
    {{Board::MUX::CurrentSense, ADC::Reference::Internal}, &Analog::current},
    {{Board::MUX::SenseRef, ADC::Reference::Internal}, &Analog::currentRef},
    // Give temperature hardware time to settle
    {{TemperatureSensorMux, ADC::Reference::Internal}, []() {}},
    {{TemperatureSensorMux, ADC::Reference::Internal}, &Analog::temperature},
};

// Is this line needed?
template class ScanningADC<numberOfScannedAnalogInputs>;

ISR(ADC_vect) { ScanningADC<numberOfScannedAnalogInputs>::interrupt(); }
#else
ISR(ADC_vect) {}
#endif