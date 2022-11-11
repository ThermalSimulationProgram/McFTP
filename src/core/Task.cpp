#include "core/Task.h"

#include <iostream>

#include "taskloads/BusyWait.h"
#include "taskloads/Benchmark.h"
#include "interfaces/UserDefinedLoad.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** CONSTRUCTOR ***********/

Task::Task(_task_type type, _task_load_type load, int loadId, int _taskId,
           int _JobId, struct timespec _relativeDeadline)
{
   task_type      = type;
   task_load_type = load;
   taskId         = _taskId;
   jobId          = _JobId;

   releaseTime      = TimeUtil::getTime();
   relativeDeadline = _relativeDeadline;
   absoluteDeadline = releaseTime + relativeDeadline;

   worker   = NULL;
   finished = false;

   printf("Inside Task::Task, task_load_type=%d\n", task_load_type);

   switch (task_load_type)
   {

   case busywait: {
      BusyWait *newload = new BusyWait();
      loads = (TaskLoad *)newload;
      break;
   }

   case benchmark: {
      printf("Creating benchmark!\n");
      Benchmark *newload = new Benchmark("hyperbolic");
      loads = (TaskLoad *)newload;
      break;
   }

   case userdefined: {
      UserDefinedLoad *newload = new UserDefinedLoad(loadId);
      loads = (TaskLoad *)newload;
      break;
   }
   }
}

Task::~Task()
{
   delete loads;
}

/*********** MEMBER FUNCTIONS ***********/

void Task::setWorker(Worker *w)
{
   worker = w;
}

void Task::setLoad(TaskLoad *l)
{
   loads = l;
}

///This function performs one task
bool Task::fire(int i)
{
   cout << "Task::fire - This should not print!\n";
   return(false);
}

unsigned long Task::getWCET(int id)
{
   cout << "Task::getWCET - This should not print!\n";
   return(0);
}

int Task::getId()
{
   return(jobId);
}

int Task::getTaskId()
{
   return(taskId);
}

_task_type Task::getType()
{
   return(task_type);
}

void Task::stop()
{
   loads->stop();
}

// void Task::suspend(const struct timespec& length){
//    loads->suspend(length);

// }

// void Task::resume(int source){
//    loads->resume(source);
// }


bool Task::isFinished()
{
   return(finished);
}

int Task::getFinishCoreId()
{
   return(worker->getId());
}

void Task::setLoadExecutionInterrupter(ExecutionInterrupter *e)
{
   loads->setExecutionInterrupter(e);
}
