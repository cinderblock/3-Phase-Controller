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


void updateCommutation() {
  unsigned int hallState = HallWatcher::getState();

  // Bail if bad hall state
  if (hallState == 0 || hallState >= 7) return;

  //Debug::dout << PSTR("at updateCommutation\r\n");
  ThreePhaseDriver::PhasePosition angle = command[hallState];

  Debug::dout << PSTR("angleBDC is ") << angle << PSTR("\n\r");

  if (forward) {
    angle += ThreePhaseDriver::StepsPerCycle / 4;
  } else {
    angle -= ThreePhaseDriver::StepsPerCycle / 4;
  }
  Debug::dout << PSTR("angle is ") << angle << PSTR("\n\r");
  // Debug::dout << PSTR("angle is ") << ThreePhaseDriver::StepsPerCycle / 4 << PSTR("\n\r");

  ThreePhaseDriver::advanceTo(angle);
}
