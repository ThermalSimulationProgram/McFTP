#ifndef _PIPELINED_H
#define _PIPELINED_H

#include <vector>

#include "core/Task.h"

/***************************************
*        CLASS DECLARATION            *
***************************************/

class Pipelined : public Task {
private:

   /*********** VARIABLES ***********/

   ///This paremeter specifies the worst-case execution time for the busy wait loop
   // std::vector<struct timespec> WCET;
   std::vector <unsigned long> wcet_us;

   std::vector <int> coreIds;

   // std::vector<bool> coreFinished;

   int nextCoreId;

public:

   /*********** CONSTRUCTOR ***********/

   ///The constructor requires a pointer
   Pipelined(const std::vector <unsigned long>& wcet, _task_load_type load, int loadId, int taskid,
             int _JobId, struct timespec _relativeDeadline);

   ~Pipelined();

   /*********** INHERITED FUNCTIONS ***********/

   /**** FROM TASK ****/

   //This function will perform one task with execution time WCET
   bool fire(int);

   int getNextCoreId();

   unsigned long getWCET(int id);
};

#endif
