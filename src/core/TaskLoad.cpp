#include "core/TaskLoad.h"


#include <iostream>

#include "core/Task.h"
#include "core/ComponentsDefine.h"

volatile double flop_a = 0.5, flop_b = 2.2;

#define NOT_FIRED          0
#define IS_EXECUTING       1
#define IS_SLEEPING        2
#define IS_READING_PAPI    3
#define IS_FINISHED        4

using namespace std;

TaskLoad::TaskLoad()
{
   // sem_init(&suspend_sem, 0, 0);
   // sem_init(&resume_sem, 0, 0);
   sem_init(&stop_sem, 0, 0);
   // sem_init(&papi_read_sem, 0, 0);
   // sem_init(&state_sem, 0, 1);
   // sem_init(&source_vector_sem, 0, 1);

   // sem_wait(&state_sem);
   // current_state = NOT_FIRED;
   // sem_post(&state_sem);

   wcet_us = 0;

   // sleepEnd.tv_sec = 0;
   //    sleepEnd.tv_nsec = 0;

   TASKLOADCOUNTER     = 0;
   TASKLOADSTOPCOUNTER = 0;

   attached_EI = NULL;



   // isInterrupted = false;
}

TaskLoad::~TaskLoad()
{
}

bool TaskLoad::LoadsInterface(unsigned long _wcet_us)
{
   initCheckCounter();
   runLoads(_wcet_us);
   return(true);
}

bool TaskLoad::runLoads(unsigned long _wcet_us)
{
   std::cout << "TaskLoad::runLoads: This should not print!\n";
   return(false);
}

// void TaskLoad::trigerPAPIReading(int source){
//    attached_EI->triggerPAPIReading(source);
// }
// void TaskLoad::suspend(const struct timespec& _sleepEndTime){
//    attached_EI->suspendExecution(_sleepEndTime);
// }

// void TaskLoad::resume(int source){
//    attached_EI->resumeExecution(source);
// }


void TaskLoad::stop()
{
   // cout << "TaskLoad stop " << endl;
   sem_post(&stop_sem);
}

void TaskLoad::dummy(void *array)
{
   (void)array;
}

void TaskLoad::do_flops(int n)
{
   int    i;
   double c = 0.11;

   for (i = 0; i < n; i++)
   {
      c += flop_a * flop_b;
   }
   dummy(( void * )&c);
}

void TaskLoad::setSuspendPoint()
{
   attached_EI->checkInterrupts();
}

void TaskLoad::setExecutionInterrupter(ExecutionInterrupter *ei)
{
   attached_EI = ei;
}
