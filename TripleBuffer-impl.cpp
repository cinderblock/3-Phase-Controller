#include <TripleBuffer.cpp>
#include "Config.h"

using namespace ThreePhaseControllerNamespace;

template class TripleBuffer<Config::i2cBufferOutgoingSize, false>;
