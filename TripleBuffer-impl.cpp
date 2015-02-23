#include "TripleBuffer.cpp"
#include "Config.h"

template class TripleBuffer<Config::i2cBufferSize, false>;
