#include <AVR++/BlockBuffer.cpp>
#include "Config.h"

using namespace ThreePhaseControllerNamespace;

template class BlockBuffer<Config::i2cBufferIncomingSize, Config::i2cBufferIncomingBlocks, true>;