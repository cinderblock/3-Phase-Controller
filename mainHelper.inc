


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



void holdForButton(){
  // Debug::dout << PSTR("Holding .\r\n");

  while (!Board::Sw1::isActive()) printHallStateIfChanged();  // hang here
  _delay_ms(100);
  while (!Board::Sw1::isActive()) printHallStateIfChanged();  // hang here
  while (Board::Sw1::isActive()) printHallStateIfChanged();  // hang here
  while (!Board::Sw1::isActive()) printHallStateIfChanged();  // hang here
  // Debug::dout << PSTR("Armed, release button to file.\r\n");

  Debug::dout << PSTR("done.\r\n");

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
