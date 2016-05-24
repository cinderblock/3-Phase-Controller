
#include "Predictor.h"
// #include "Debug.h"
#include <util/atomic.h>
#include "DriverConstants.h"
#include <cmath> 

using namespace std;

u4 Predictor::drivePhase;
u2 Predictor::lastMecPha;
s2 Predictor::driveVelocity;
s2 Predictor::lastMechChange;

u2 Predictor::predict(){

 u4 ph = drivePhase;
 ph += driveVelocity;
 
 const bool forward = driveVelocity > 0;
 
 const u4 MAX = DriverConstants::StepsPerCycle << DriverConstants::drivePhaseValueShift;
 
 // Check if ph(ase) value is out of range
 if (ph > MAX) {
  // Fix it
  if (forward)
   ph -= MAX;
  else
   ph += MAX;
 }
 
 // Store new drivePhase
 drivePhase = ph;
 
 // Adjust output for velocity lag
 ph += driveVelocity * driveVelocityPhaseAdvance;
 
  // Check if ph(ase) value is out of range again
 if (ph > MAX) {
  // Fix it
  if (forward) ph -= MAX;
  else         ph += MAX;
 }
 
 return (ph >> DriverConstants::drivePhaseValueShift) & DriverConstants::BitsForPhase;
}

void Predictor::freshPhase(u2 phase){

 u2 mechanicalPhase = getMechPhase(phase);

 // auto tempVelocity = driveVelocity;
 
 
 s2 mechChange = mechanicalPhase - lastMecPha; 
 
 if(abs(mechChange - lastMechChange) > DriverConstants::MaxVelocityChange){
  mechChange = DriverConstants::StepsPerRotation - mechanicalPhase + lastMecPha;
 }

 lastMechChange = mechChange;

 auto tempVelocity = nextVelocity(driveVelocity, mechChange);
 
 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  driveVelocity = tempVelocity;
  drivePhase = u4(phase & DriverConstants::BitsForPhase) << DriverConstants::drivePhaseValueShift;
 }
 
 // static u1 tick = 0;

 // Debug::SOUT
 //         << Debug::Printer::Special::Start
 //         << tick++
 //         << mechanicalPhase
 //         << Debug::Printer::Special::End;

 // Save the most recent magnetic position
 lastMecPha = mechanicalPhase;
 
}

const u1 shiftVal = 100;

s4 Predictor::nextVelocity(s4 tempVelocity, s2 measuredMechChange){

 const s2 predictedPhaseChange = (s4(tempVelocity) * DriverConstants::PredictsPerValue) >> DriverConstants::drivePhaseValueShift;

 //TODO make this actually reflect max acceleration
 if (measuredMechChange > predictedPhaseChange) {
  tempVelocity+=shiftVal;
 } else if (measuredMechChange < predictedPhaseChange) {
  tempVelocity-=shiftVal;
 }

 return tempVelocity;
}

void Predictor::init(u2 phase){

 driveVelocity = 0;
 lastMecPha = getMechPhase(phase);//lookupAlphaToPhase(MLX90363::getAlpha());
 drivePhase = lastMecPha << DriverConstants::drivePhaseValueShift;
 lastMechChange = 0;
}
