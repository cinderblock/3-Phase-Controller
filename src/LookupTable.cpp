#include "LookupTable.hpp"
#include <AVR++/FlashCRC.hpp>

using namespace ThreePhaseControllerNamespace;

const bool Lookup::isValid = AVR::FlashCRC16(Location, Location + Size * 2 + 2) == 0;