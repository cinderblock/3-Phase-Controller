#include "Board.hpp"
#include "main.hpp"
#include <AVR++/ScanningADC.cpp>

using namespace ThreePhaseControllerNamespace;

static constexpr u1 numberOfScannedAnalogInputs = 3;

template <u1 N>
typename ScanningADC<N>::Input ScanningADC<N>::inputs[] = {
    {Board::MUX::AS, &Analog::AS},
    {Board::MUX::BS, &Analog::BS},
    {Board::MUX::CS, &Analog::CS},
};

// Is this line needed?
template class ScanningADC<numberOfScannedAnalogInputs>;

ISR(ADC_vect) { ScanningADC<numberOfScannedAnalogInputs>::interrupt(); }