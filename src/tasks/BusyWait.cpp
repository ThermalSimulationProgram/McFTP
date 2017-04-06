#include "tasks/BusyWait.h"

#include <iostream>

#include "core/Dispatcher.h"
#include "core/CMI.h"
#include "utils/Operators.h"
#include "utils/TimeUtil.h"



using namespace std;

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

///The constructor requires a pointer to the simulation, its own dispatcher, and the WCET
BusyWait::BusyWait(struct timespec wcet) : Task(busy_wait) {
  WCET = wcet;
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM TASK ****/

void BusyWait::fire() {
  unsigned int start, end, count=0, wcet_us = TimeUtil::convert_us(WCET);
  
  start = TimeUtil::convert_us(TimeUtil::getTime());

  //BUSY WAIT LOOP
  do {
    end = TimeUtil::convert_us(TimeUtil::getTime());

    if( (end - start) > 100) {
      setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
      count += 100;
      start = end;
    }
  } while ( count < wcet_us && CMI::isRunning()); 
}
