#ifndef EXECUTION_INTERRUPTER_H
#define EXECUTION_INTERRUPTER_H



#include <time.h>
#include <atomic>
#include <semaphore.h>
#include <list>

#include "utils/TimeUtil.h"
#include "utils/Operators.h"

#define NOT_FIRED       0
#define IS_EXECUTING    1
#define IS_SLEEPING     2

class Worker;

class ExecutionInterrupter {
protected:
   sem_t suspend_sem;

   sem_t resume_sem;

   sem_t papi_read_sem;

   sem_t state_sem;

   int current_state;

   std::list <int> resume_sem_sources;

   sem_t source_vector_sem;

   struct timespec sleepEnd;

   Worker *worker;

public:
   ExecutionInterrupter();

   void setWorker(Worker *w);

   void informExecutionBegin();

   void checkInterrupts();

   void suspendExecution(const struct timespec &length);

   void resumeExecution(int source);

   void triggerPAPIReading(int source);
};


#endif
