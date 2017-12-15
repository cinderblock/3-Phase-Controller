

// Use this to test the operation of the Hall sensors.
// It will get stuck here.
// state 7 and 0 are invalid.
// 0 suggests device is not plugged in.
// Sequence is 6,4,5,1,3,2 or reverse.
void displayHallState() {
  u1 lastState = 0;

  while (1) {
    u1 newState = HallWatcher::getState();
    if (newState == lastState) continue;
    lastState = newState;
    Debug::dout << lastState << PSTR(" is hall state\r\n");
  }
}
