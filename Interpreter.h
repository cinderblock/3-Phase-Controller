
#ifndef INTERPRETER_H
#define	INTERPRETER_H

#include "Config.h"

class Interpreter {
  static bool checkCRC(u1 const * const);
  static u1 getCRC(u1 const * const, u1);

  static bool streaming;

public:
  static void interpretFromMaster(u1 const * const);

  static void sendNormalDataToMaster();
};

#endif	/* INTERPRETER_H */
