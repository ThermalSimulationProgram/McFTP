#ifndef _BUSYWAIT_H
#define _BUSYWAIT_H

#include <vector>

#include "core/Task.h"

/***************************************
 *        CLASS DECLARATION            * 
 ***************************************/

class BusyWait : public Task {

 private:

  /*********** VARIABLES ***********/

  ///This paremeter specifies the worst-case execution time for the busy wait loop
  std::vector<struct timespec> WCET;
  std::vector<unsigned long> wcet_us;

  std::vector<int> coreIds;

  std::vector<bool> coreFinished;

  int nextCoreId;

 public:

  /*********** CONSTRUCTOR ***********/

  ///The constructor requires a pointer
  BusyWait(const std::vector<struct timespec>& wcet, const std::vector<int>& _coreIds, int taskid);

  ~BusyWait();

  /*********** INHERITED FUNCTIONS ***********/
  
  /**** FROM TASK ****/
  
  //This function will perform one task with execution time WCET
  void fire();

  int getNextCoreId();
};

#endif

