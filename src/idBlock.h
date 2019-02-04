#pragma once

#include <basicTypes.h>
#include <stddef.h>
#include <stdint.h>

namespace ThreePhaseControllerNamespace {
namespace ID {

constexpr size_t address = 0x6f80;
constexpr size_t length = 128;

namespace usbSerial {
constexpr size_t address = ID::address + 11;
}

extern const bool valid;

using namespace Basic;

using StoredData = union {
  struct {
    u2 calibrationCRC;
    u1 idVersion;
    s8 calibrationTime;
    u1 usbID[115];
    u2 CRC;
  };
  u1 idData[128];
};

} // namespace ID
} // namespace ThreePhaseControllerNamespace