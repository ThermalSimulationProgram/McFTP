#include "tasks/SingleCore.h"

#include <iostream>
#include <cstdlib>

// #include "core/Dispatcher.h"
// #include "core/CMI.h"
// #include "core/Worker.h"
// #include "utils/Operators.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
// #include "utils/vectormath.h"
#include "utils/utils.h"


using namespace std;


SingleCore::SingleCore(unsigned long _wcet_us, _task_load_type load, int loadId,
                       int taskid, int _JobId, struct timespec _relativeDeadline) :
   Task(singlecore, load, loadId, taskid, _JobId, _relativeDeadline)
{
   wcet_us = _wcet_us;
}

bool SingleCore::fire(int i)
{
	// printf("Inside singlecore::fire, id=%d\n", i);
   finished = loads->runLoads(wcet_us);
   return(finished);
}
