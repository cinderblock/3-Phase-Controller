
#include "Clock.h"
#include <math.h>

void printHallStateIfChanged();
void doSinePulse();




// This ditty commands a sequence of outputs to make a demo pulse to the bed
void doSinePulse(s2 amplitude){
  constexpr float freq = 2;
  constexpr float maxTime = 1 * 1000; //
  constexpr float PI = 3.14159;
  constexpr float omegaTerm = 1000 / (2*PI*freq);

  constexpr u2 maxAmpl = 255 - 30;

  if (amplitude >  maxAmpl) amplitude =  maxAmpl;
  if (amplitude < -maxAmpl) amplitude = -maxAmpl;

  Clock::MicroTime nextTime;
  Clock::readTime(nextTime);

  u2 step = 0;


  do {
    nextTime += 1_ms;
    while (!nextTime.isInPast()) {}
    //   // Do things here while we're waiting for the 1kHz tick
    step += 1;
    int bang = amplitude * sin (step/omegaTerm);
    Debug::dout << bang << PSTR("\r\n");

    ThreePhaseDriver::setAmplitude(bang);

  } while (step < maxTime);

} // sin test



// Use this to test the operation of the Hall sensors.
// It will get stuck here.
// state 7 and 0 are invalid.
// 0 suggests device is not plugged in.
// Sequence is 6,4,5,1,3,2 or reverse.
void displayHallState() {
  while (1) {
    printHallStateIfChanged();
  }
}



void printHallStateIfChanged() {
  u1 static lastState = HallWatcher::getState();

  const u1 state = HallWatcher::getState();

  if (state == lastState) return;

  lastState = state;
  Debug::dout << PSTR("New Hall State: ") << state << ' ' << MLX90363::getAlpha() << '\r' << '\n';
}


// Get stuck here waiting for a button press.
// Make sure you keep outputting Hall state chanes (if thast is needed).
// Once a button presss is detected, wait a bit to make sure it is real.
// Then you can go back to let the next thing happen.
void holdUntilButtonIsStateForPeriodAndDoThing(bool active, Clock::MicroTime const debounce = 20_ms, void (*thingToDoWhileWeWait)() = &printHallStateIfChanged) {

  Clock::MicroTime returnTime;

  while(true) {

    // button has not been pressed yet
    while (Board::Sw1::isActive() != active) thingToDoWhileWeWait();  // hang here but keep checking halls
    // whoo hoo something has happened on the button.

    Clock::readTime(returnTime);
    returnTime += debounce;

    do {
      if (returnTime.isInPast()) return;  //  all done
      thingToDoWhileWeWait();
    } while (Board::Sw1::isActive() == active);

    //  If we get here, that means button was released before debounce period. Loop
  }

}

void holdUntilButtonFallingEdge() {
  holdUntilButtonIsStateForPeriodAndDoThing(false, 50_ms, &printHallStateIfChanged);
  holdUntilButtonIsStateForPeriodAndDoThing(true, 50_ms, &printHallStateIfChanged);  // down time
}


// here is a buch of fragments used when we were debugging the halls:


// holdUntilButtonIsState();
//doSquarePulse();

// signed int command = 10;


// ThreePhaseDriver::setDeadTimes(0xff);
// ThreePhaseDriver::setAmplitude(255); // out of -255 to 255



// u2 hallTest = ThreePhaseDriver::StepsPerCycle / 4;
// ThreePhaseDriver::advanceTo(704);  // go to this particular output state
// u1 deadTest = 0;
// u2 constexpr fixedAngle = 704;
// hallTest = fixedAngle;


// setOverrideHallState(hallTest);
// Debug::dout << hallTest << PSTR("\r\n");
// ThreePhaseDriver::advanceTo(hallTest);  // go to this particular output state

// hallTest -= 128/4;
// while (hallTest >= 768) hallTest += 768;

//updateCommutation();

// Debug::dout << (deadTest & 0xf) << PSTR("\r\n");
// ThreePhaseDriver::setDeadTimes(deadTest);
// deadTest += 0x11;  // increment both nibbles

// hallTest = ((hallTest != 704) ? 704 : 576);

// _delay_ms(4);


// hallTest += 3;
// while (hallTest >= 768) hallTest -= 768;
//hallTest -= 3;
//while (hallTest <= 0) hallTest += 768;

//
// if (state % 2 == 0){ //even
//   Board::LED1::on();
//   Debug::dout << state << PSTR(" is even\r\n");
// }else{
//   Board::LED1::off();
//   Debug::dout << state  << PSTR(" is odd\r\n");
// }


// if (Calibration::enabled) {
//   Calibration::main();
// }
// else
// if (Demo::enabled) {
//   Demo::main();
// }
// else {
//   // Init for hardware interface.
//   ServoController::init();
//
//   //main loop
//   while (1) {
//     // Let ServoController calculate new amplitude command
//     ServoController::update();
//
//
//   }
// }
