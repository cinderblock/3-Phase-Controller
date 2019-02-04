#include "LookupTable.h"
#include <AVR++/FlashCRC.h>

using namespace ThreePhaseControllerNamespace;

const bool Lookup::isValid = AVR::FlashCRC16(Location, Location + Size * 2 + 2) == 0;