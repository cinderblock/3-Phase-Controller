#pragma once

namespace ThreePhaseControllerNamespace {

using statusType = struct {
  bool calibrated : 1;
  bool fault : 1;
};

extern statusType status;

} // namespace ThreePhaseControllerNamespace