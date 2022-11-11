#ifndef _TASK_H
#define _TASK_H


#include <time.h>
#include <string>
#include <semaphore.h>

#include "utils/Enumerations.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "results/Statistics.h"
#include "core/Worker.h"
#include "core/TaskLoad.h"

/***************************************
*        CLASS DECLARATION            *
***************************************/


class ExecutionInterrupter;

class Task {
protected:

   /*********** VARIABLES ***********/

   ///Varible describing the type of task (pipelined, singlecore)
   _task_type task_type;
   ///Varible describing the task's type of load (busywait, benchmarks, userdefined)
   _task_load_type task_load_type;

   // The index of this kind of task
   int taskId;
   // The index of this entity of the task
   int jobId;

   struct timespec releaseTime;

   struct timespec relativeDeadline;

   struct timespec absoluteDeadline;

   // the worker which is currently running this task
   Worker *worker;

   // task load
   TaskLoad *loads;

   // variable indicating whether this job has finished
   bool finished;

public:

   /*********** CONSTRUCTOR ***********/

   ///Constructor needs its load type
   Task(_task_type type, _task_load_type load, int loadId, int _taskId,
        int JobId, struct timespec _relativeDeadline);

   virtual ~Task();

   /*********** MEMBER FUNCTIONS ***********/

   void setWorker(Worker *w);

   ///This function performs one task
   virtual bool fire(int);

   void stop();

   void setLoad(TaskLoad *l);

   // void suspend(const struct timespec& length);

   // void resume(int source);

   int getId();

   _task_type getType();

   int getTaskId();

   bool isFinished();

   int getFinishCoreId();

   virtual unsigned long getWCET(int id);

   void setLoadExecutionInterrupter(ExecutionInterrupter *e);
};
#endif
