
#include "Predictor.h"
// #include "Debug.h"
#include <util/atomic.h>
#include "DriverConstants.h"
// #include <iostream>
// #include <cmath> 

using namespace std;

u4 Predictor::drivePhase;
u2 Predictor::lastMecPha;
s2 Predictor::driveVelocity;
s2 Predictor::lastMechChange;
u2 Predictor::lastReading;


s2 abs(s2 num){
  if(num < 0) return -num;
  return num;
}

u2 Predictor::predict(){

 u4 ph = drivePhase;
 ph += driveVelocity;
 
 const bool forward = driveVelocity > 0;
 
 static const u4 MAX = DriverConstants::StepsPerCycle << DriverConstants::drivePhaseValueShift;
 
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

 // return ph;
 // return (ph>>DriverConstants::drivePhaseValueShift)% DriverConstants::StepsPerCycle;

 // // Adjust output for velocity lag
 // ph += driveVelocity * driveVelocityPhaseAdvance;
 
 //  // Check if ph(ase) value is out of range again
 // if (ph > MAX) {
 //  // Fix it
 //  if (forward) ph -= MAX;
 //  else         ph += MAX;
 // }
 
 return (ph >> DriverConstants::drivePhaseValueShift) & DriverConstants::BitsForPhase;
}

void Predictor::freshPhase(u2 reading){

 // static u1 tick = 0;

 // Debug::SOUT
 //         << Debug::Printer::Special::Start
 //         << tick++
 //         << reading
 //         << Debug::Printer::Special::End;


 u2 mechanicalPhase = getMechPhase(reading);

 // auto tempVelocity = driveVelocity;
 
 
 s2 mechChange = mechanicalPhase - lastMecPha; 
 
 //TODO ensure we are not wrapping in the wrong direction due to high speeds
 
 if (mechChange > DriverConstants::StepsPerRotation/2){
  mechChange = DriverConstants::StepsPerRotation - mechChange;
 }
 else if (mechChange < -((s2)DriverConstants::StepsPerRotation/2)){
  mechChange = DriverConstants::StepsPerRotation + mechChange;
 }

 auto tempVelocity = nextVelocity(driveVelocity, mechChange);

 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  driveVelocity = tempVelocity;
  // lastReading = reading;
  drivePhase = u4(reading & DriverConstants::BitsForPhase) << DriverConstants::drivePhaseValueShift;
 }
 
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
 drivePhase = (lastMecPha & DriverConstants::BitsForPhase) << DriverConstants::drivePhaseValueShift;
 lastMechChange = 0;
}
