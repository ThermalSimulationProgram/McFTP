#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include <time.h>
#include <vector>
#include <semaphore.h>

#include "pthread/Thread.h"
#include "utils/Enumerations.h"
#include "core/Task.h"
#include "core/TaskArgument.h"
#include "core/structdef.h"

class Processor;

/***************************************
*        CLASS DECLARATION            *
***************************************/

class Dispatcher : public Thread {
protected:

   /*********** VARIABLES ***********/

   ///Periodicity of the task to be dispatched
   _task_periodicity periodicity;

   ///When this is set, the dispatcher will sleep for the specified time before beginning to dispatch
   struct timespec offset;

   ///Pointer to the Processor associated to the dispatcher
   Processor *processor;

   std::vector <Task *> allTasks;

   _task_load_type TASK_LOAD_TYPE;

   _task_type TASK_TYPE;

   TaskArgument taskdata;

   int jobCounter;

   sem_t dispatch_sem;

public:

   /*********** CONSTRUCTOR ***********/

   ///Constructor needs  a disp_id
   Dispatcher(unsigned int id);

   ~Dispatcher();

   /*********** INHERITED FUNCTIONS ***********/

   /**** FROM THREAD ****/

   ///This join function takes into account the dispatcher's unblocking mechanism
   void join();

   ///This is the pthread's wrapper function, calls dispatch -> which has the flagged loop.
   void wrapper();

   /*********** MEMBER FUNCTIONS ***********/

   ///This function assignes DISP_PR to the thread
   void activate();

   ///This function was a flagged loop that activates the Worker according to the task periodicity
   virtual void dispatch();

   /*********** SETTER FUNCTIONS ***********/

   ///This function sets the dispatcher's offset
   void setOffset(struct timespec o);

   ///This function sets the tasks's periodicity
   void setPeriodicity(_task_periodicity t);

   void setTaskLoadType(_task_load_type type);

   void setTaskType(_task_type type);

   void setTaskData(TaskArgument d);

   ///This function sets the associated CMI
   void setProcessor(Processor *c);

   Task *createNewTask();

   int getTaskId();
};

#endif
