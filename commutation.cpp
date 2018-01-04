#include "commutation.h"
#include "ThreePhaseDriver.h"
#include "HallWatcher.h"
#include "Debug.h"


volatile bool forward = true;


// ThreePhaseDriver::PhasePosition convertHallStateToCameronAngle(u1 state) {
//     // we need something to specify direction
//     // from hoverboard code:
//     // see excel
//     // int commandR[8] = {0, 20, 33, 17, 10, 12, 34, 0 };  // counter clockwise
//     // int commandF[8] = {0, 34, 12, 10, 17, 33, 20, 0 };  // clockwise
//     //                    0   1   2   3   4   5   6  7
//     static ThreePhaseDriver::PhasePosition command[8] = {0, 256,0,64,448,320,576,0}
//
//    return ;
// }
using namespace ThreePhaseControllerNamespace;


static const ThreePhaseDriver::PhasePosition command[] = {0, 256,0,128,512,384,640};

void setPWM(signed int amplitude) {
  if (amplitude < 0) {
    ThreePhaseDriver::setAmplitude(-amplitude);
    forward = false;
  } else {
    ThreePhaseDriver::setAmplitude(amplitude);
    forward = true;
  }
  updateCommutation();
}

volatile u1 overrideHallState = 0;

void setOverrideHallState(u1 state) {
  overrideHallState = state;
}

void updateCommutation() {
  // disable this function for now to test other things
  return;



  unsigned int hallState = HallWatcher::getState();

  if (overrideHallState) {
    hallState = overrideHallState;
  }

  // Bail if bad hall state
  if (hallState == 0 || hallState >= 7) return;

  //Debug::dout << PSTR("at updateCommutation\r\n");
  ThreePhaseDriver::PhasePosition angle = command[hallState];
  //angle = 256;  // force to a fixed value for testing.

  Debug::dout << PSTR("Converted hall state is angle: ") << angle << PSTR("\r\n");

  if (forward) {
    angle += ThreePhaseDriver::StepsPerCycle / 4;
  } else {
    angle -= ThreePhaseDriver::StepsPerCycle / 4;
  }
  Debug::dout << PSTR("Angle with 90deg phase shift:  ") << angle << PSTR("\r\n");
  // Debug::dout << PSTR("angle is ") << ThreePhaseDriver::StepsPerCycle / 4 << PSTR("\r\n");

  ThreePhaseDriver::advanceTo(angle);
}
