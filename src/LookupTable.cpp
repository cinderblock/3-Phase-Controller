#include "LookupTable.h"

#include "Config.h"

using namespace Basic;
using namespace ThreePhaseControllerNamespace;

/**
 * 12-bit lookup table for magnetometer Alpha value to Phase value
 */
const u2 Lookup::table[] PROGMEM = {
#include "Calibration.csv"
};
