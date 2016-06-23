#include <AVR++/BlockBuffer.cpp>
#include "Config.h"

template class BlockBuffer<Config::i2cBufferIncomingSize, Config::i2cBufferIncomingBlocks, true>;