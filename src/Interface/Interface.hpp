#pragma once

#include "PacketFormats.h"

namespace ThreePhaseControllerNamespace {
void handleIncomingCommand(const CommandFormat &);
void packOutputDataPacket(DataFormat &);
}; // namespace ThreePhaseControllerNamespace
