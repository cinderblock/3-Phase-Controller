
#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "Predictor.h"
#include <AVR++/basicTypes.h>

using namespace AVR;

class Predictor{
  // static constexpr u1 PredictsPerValue = 40;

  static u2 lastMagPha;
  static u4 drivePhase;
  static s2 driveVelocity;
  static constexpr u1 driveVelocityPhaseAdvance = 1;

public:
  static void freshPhase(u2 phase);
  static void init(u2);
  static u2 predict();
  static s4 nextVelocity(s4, s2);

  inline static u4 getPredictedPosition(){return drivePhase;}
  inline static u2 getMeasuredPosition() {return lastMagPha;}
  inline static s2 getVelocity()         {return driveVelocity;}
};

#endif  /* PREDICTOR_H */
