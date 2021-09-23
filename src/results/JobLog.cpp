#include "results/JobLog.h"


JobLog::JobLog(int _workerid, int _taskid) :
   workerId(_workerid), taskId(_taskid)
{
};

int JobLog::getWorkerId()
{
   return(workerId);
}

int JobLog::getTaskId()
{
   return(taskId);
}

bool JobLog::isMatch(int _workerid, int _taskid)
{
   if (workerId == _workerid && taskId == _taskid)
   {
      return(true);
   }
   else
   {
      return(false);
   }
}
