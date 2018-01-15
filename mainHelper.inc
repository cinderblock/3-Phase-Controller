
#include "Clock.h"

void printHallStateIfChanged();



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
  Debug::dout << PSTR("New Hall State: ") << state << '\r' << '\n';
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
