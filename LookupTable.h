/* 
 * File:   LookupTable.h
 * Author: Cameron
 *
 * Created on June 20, 2016, 5:39 PM
 */

#ifndef LOOKUPTABLE_H
#define	LOOKUPTABLE_H

#include <avr/pgmspace.h>
#include <AVR++/basicTypes.h>

namespace ThreePhaseControllerNamespace {

using namespace AVR;

class Lookup {
public:
  /**
   * Number of bits used by input
   */
  static constexpr u1 InputBits = 14;

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

  /**
   * Converts the magnetometer to a phase value
   * @param alpha 14-bit value from magnetometer
   * @return phase upperNibble: mechanical pos. Rest: value (0 - 0x2ff inclusive)
   */
  inline static u2 AlphaToPhase(u2 alpha) {
    // Make sure we're working with a 14-bit number
    alpha &= ((1 << InputBits) - 1);

    //divide alpha by 4 to get a reasonable table size
    alpha >>= ResolutionReductionBits;

    // Read the phase number word from the calculated place in the lookup table
    return pgm_read_word(&Lookup::table[alpha]);
  }


private:
  static const u2 table[Size] PROGMEM;
};

};

#endif	/* LOOKUPTABLE_H */
