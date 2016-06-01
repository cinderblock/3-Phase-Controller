
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
u1 Predictor::adjustVal;
u1 Predictor::driveVelocityPhaseAdvance;
// u1 Predictor::ratio;

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

 auto tempVelocity = nextVelocity(mechChange);

 ATOMIC_BLOCK(ATOMIC_FORCEON) {
  driveVelocity = tempVelocity;
  // lastReading = reading;
  drivePhase = u4(reading & DriverConstants::BitsForPhase) << DriverConstants::drivePhaseValueShift;
 }
 
 // Save the most recent magnetic position
 lastMecPha = mechanicalPhase;
 
}

const u1 adjustVal = 60;

// const u1 smoothingType = 1;

//With the data set we had, 60 seemed to be the optimal value for adjustments
//however other smoothing methods were better in certain conditions. More research is required
s4 Predictor::nextVelocity(s2 measuredMechChange){

 s4 tempVelocity;

 // if (smoothingType == 1){
 tempVelocity = driveVelocity;

 const s2 predictedPhaseChange = (s4(tempVelocity) * DriverConstants::PredictsPerValue) >> DriverConstants::drivePhaseValueShift;

 if (measuredMechChange > predictedPhaseChange) {
  tempVelocity+=adjustVal;
 } else if (measuredMechChange < predictedPhaseChange) {
  tempVelocity-=adjustVal;
 }
 // }
 // else if(smoothingType == 2){
 //  static s4 n1 = 0;
 //  static s4 n2 = 0;
 //  static s4 n3 = 0;

 //  s4 temp = (s4)measuredMechChange << DriverConstants::drivePhaseValueShift;

 //  tempVelocity = (n3+3*n2+n1*6+temp*7)/16;

 //  tempVelocity /= DriverConstants::PredictsPerValue;

 //  n3 = n2;
 //  n2 = n1;
 //  n1 = temp;
 // }
 // else if(smoothingType == 3){
 //  // static s4 last = 0;
 //  // const u1 ratio = 10;

 //  // s4 curr = ((s4)measuredMechChange << DriverConstants::drivePhaseValueShift);
 
 //  tempVelocity = (driveVelocity>>DriverConstants::drivePhaseValueShift)*ratio+((s4)measuredMechChange)*((1<<DriverConstants::drivePhaseValueShift) - ratio);

 //  // last = tempVelocity;

 //  tempVelocity /= DriverConstants::PredictsPerValue;
 // }
 
 return tempVelocity;
}

void Predictor::init(u2 phase){

 driveVelocity = 0;
 lastMecPha = getMechPhase(phase);//lookupAlphaToPhase(MLX90363::getAlpha());
 drivePhase = (lastMecPha & DriverConstants::BitsForPhase) << DriverConstants::drivePhaseValueShift;
 lastMechChange = 0;
 adjustVal = 5;
 driveVelocityPhaseAdvance = 1;
}

