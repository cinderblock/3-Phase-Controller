/*
 * File:   LookupTable.h
 * Author: Cameron
 *
 * Created on June 20, 2016, 5:39 PM
 */

#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <AVR++/FlashArray.h>
#include <AVR++/basicTypes.h>
#include <avr/pgmspace.h>

#include "ThreePhase/Driver.h"

namespace ThreePhaseControllerNamespace {

using namespace Basic;

class Lookup {
public:
  /**
   * Number of bits used by input
   */
  static constexpr u1 InputBits = 14;

  /**
   * Number of bits used by input
   */
  static constexpr u2 InputMask = (1 << InputBits) - 1;

  /**
   * Number of bits we reduce the input by
   */
  static constexpr u1 ResolutionReductionBits = 2;

  /**
   *  Number of bits needed to address the entire range of the table
   */
  static constexpr u1 TableBits = InputBits - ResolutionReductionBits;

  /**
   *  Number of entries in the table
   */
  static constexpr u2 Size = 1 << TableBits;

private:
  // See Boot map.svg
  static constexpr size_t Location = 0x4F80;

  static const AVR::FlashArray<Location, Size, u2> table;

public:
  static bool const isValid;

  /**
   * Converts the magnetometer to a phase value
   * @param alpha 14-bit value from magnetometer
   * @return phase upperNibble: mechanical pos. Rest: value (0 - 0x2ff inclusive)
   */
  inline static ThreePhaseDriver::PhasePosition AlphaToPhase(u2 alpha) {
    // Make sure we're working with a 14-bit number
    alpha &= InputMask;

    // divide alpha by 4 to get a reasonable table size
    alpha >>= ResolutionReductionBits;

    // Read the phase number word from the calculated place in the lookup table
    return table[alpha];
  }
};

}; // namespace ThreePhaseControllerNamespace

#endif /* LOOKUPTABLE_H */
