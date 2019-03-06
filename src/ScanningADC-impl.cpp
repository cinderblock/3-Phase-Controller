#include "Board.hpp"
#include "main.hpp"
#include <AVR++/ScanningADC.cpp>

using namespace ThreePhaseControllerNamespace;

static constexpr u1 numberOfScannedAnalogInputs = 9;

template <u1 N>
typename ScanningADC<N>::Input ScanningADC<N>::inputs[] = {
    {Board::MUX::AS, &Analog::AS},
    {Board::MUX::BS, &Analog::BS},
    {Board::MUX::CS, &Analog::CS},
    {Board::MUX::VBATS, &Analog::battery},
    {Board::MUX::VDDS, &Analog::drive},
    {Board::MUX::CurrentSense, &Analog::current},
    {Board::MUX::SenseRef, &Analog::currentRef},
    // Give temperature hardware time to settle
    {{0b100111, ADC::Reference::Internal}, []() {}},
    {{0b100111, ADC::Reference::Internal}, &Analog::temperature},
};

// Is this line needed?
template class ScanningADC<numberOfScannedAnalogInputs>;

ISR(ADC_vect) { ScanningADC<numberOfScannedAnalogInputs>::interrupt(); }