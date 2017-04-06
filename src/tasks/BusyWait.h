#ifndef _BUSYWAIT_H
#define _BUSYWAIT_H

#include "core/Task.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class BusyWait : public Task {

 private:

  /*********** VARIABLES ***********/

  ///This paremeter specifies the worst-case execution time for the busy wait loop
  struct timespec WCET;

 public:

  /*********** CONSTRUCTOR ***********/

  ///The constructor requires a pointer
  BusyWait(struct timespec wcet);

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM TASK ****/
  
  //This function will perform one task with execution time WCET
  void fire();
};

#endif

