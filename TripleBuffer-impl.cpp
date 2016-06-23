#include <AVR++/TripleBuffer.cpp>
#include "Config.h"

template class TripleBuffer<Config::i2cBufferOutgoingSize, false>;
