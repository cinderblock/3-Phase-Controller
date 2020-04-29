
#include "idBlock.hpp"
#include <AVR++/FlashCRC.hpp>

using namespace ThreePhaseControllerNamespace;

// Check ID block CRC on startup
const bool ID::valid = AVR::FlashCRC16(ID::address, ID::address + ID::length) == 0;