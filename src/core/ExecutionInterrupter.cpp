#include "core/ExecutionInterrupter.h"

#include "core/Worker.h"
#include "core/ComponentsDefine.h"


#include <iostream>
using namespace std;

#define DEBUG    0

ExecutionInterrupter::ExecutionInterrupter()
{
   sem_init(&suspend_sem, 0, 0);
   sem_init(&resume_sem, 0, 0);
   sem_init(&papi_read_sem, 0, 0);
   sem_init(&state_sem, 0, 1);
   sem_init(&source_vector_sem, 0, 1);

   sem_wait(&state_sem);
   current_state = NOT_FIRED;
   sem_post(&state_sem);

   sleepEnd.tv_sec  = 0;
   sleepEnd.tv_nsec = 0;

   worker = NULL;
}

void ExecutionInterrupter::setWorker(Worker *w)
{
   worker = w;
}

void ExecutionInterrupter::informExecutionBegin()
{
   sem_wait(&state_sem);
   current_state = IS_EXECUTING;
   sem_post(&state_sem);
}

void ExecutionInterrupter::checkInterrupts()
{
   // cout << "ExecutionInterrupter::checkInterrupts in" << endl;
   // we are entering a critical section, blocking the state
   sem_wait(&state_sem);
   bool is_state_changed = false;

   if (sem_trywait(&papi_read_sem) == 0)
   {
                #if DEBUG == 1
      cout << "exeInter: 0 \n";
                #endif
      worker->readPAPIValues();           // this function should cause little overhead
   }

   if (sem_trywait(&suspend_sem) == 0)   //successfully read a suspend singal, go to sleep immediately
   {
      current_state    = IS_SLEEPING;
      is_state_changed = true;
      // we can determine the state is sleeping, releasing the lock
      sem_post(&state_sem);

      // two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
      bool stop_sleep = false;
      do
      {
                #if DEBUG == 1
         cout << "exeInter: 1 start sleeping \n";
                #endif
         sem_wait(&resume_sem);
         // we need to determine the next state, entering the critical section again
         sem_wait(&state_sem);


         sem_wait(&source_vector_sem);
         int sem_source = resume_sem_sources.front();
         resume_sem_sources.pop_front();
         sem_post(&source_vector_sem);

         if (sem_source == TEMPWATCHER)
         {
                                #if DEBUG == 1
            cout << "exeInter: read papi values \n";
                                #endif
            current_state = IS_SLEEPING;
            worker->readPAPIValues();
         }
         else
         {
                                #if DEBUG == 1
            cout << "exeInter: exiting checker - " << sem_source << endl;
                                #endif
            stop_sleep    = true;
            current_state = IS_EXECUTING;
         }

         //now we have the next state, release the lock
         sem_post(&state_sem);
      }while (!stop_sleep);
   }

   if (!is_state_changed)
   {
      // nothing happened, release the lock
      current_state = IS_EXECUTING;
      sem_post(&state_sem);
   }
   // cout << "ExecutionInterrupter::checkInterrupts out" << endl;
}

void ExecutionInterrupter::suspendExecution(const struct timespec &_sleepEndTime)
{
   sleepEnd = _sleepEndTime;
        #if DEBUG == 1
   cout << "exeInter: suspend execution!" << endl;
        #endif
   sem_post(&suspend_sem);
}

void ExecutionInterrupter::resumeExecution(int source)
{
   sem_wait(&source_vector_sem);
   resume_sem_sources.push_back(source);
   // cout << "reseme from " << source << endl;
   sem_post(&source_vector_sem);

   sem_post(&resume_sem);
}

void ExecutionInterrupter::triggerPAPIReading(int source)
{
   sem_wait(&state_sem);
   if (current_state == IS_EXECUTING)
   {
      sem_post(&papi_read_sem);
   }
   else if (current_state == IS_SLEEPING)
   {
      resumeExecution(source);
   }
   sem_post(&state_sem);
}
