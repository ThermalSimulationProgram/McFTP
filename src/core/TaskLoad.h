#ifndef TASKLOAD_H
#define TASKLOAD_H

#include <time.h>
#include <atomic>
#include <semaphore.h>
#include <list>
#include <iostream>

#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "core/ExecutionInterrupter.h"


#define setCheckBlockBegin()                                                                             \
   ++TASKLOADCOUNTER;                                                                                    \
   if (sem_trywait(&stop_sem) == 0) {                                                                    \
      TASKLOADSTOPCOUNTER = TASKLOADSTOPCOUNTER > TASKLOADCOUNTER? TASKLOADSTOPCOUNTER: TASKLOADCOUNTER; \
      return(false);                                                                                     \
   }                                                                                                     \
   if (TASKLOADCOUNTER >= TASKLOADSTOPCOUNTER) {                                                         \




#define setCheckBlockEnd() \
}                          \


// #define NUM_SEM_SOURCE 2


class TaskLoad {
protected:

   // sem_t suspend_sem;

   // sem_t resume_sem;

   //    sem_t papi_read_sem;

   //    sem_t state_sem;

   //    int current_state;

   //    std::list<int> resume_sem_sources;

   //    sem_t source_vector_sem;

   // struct timespec sleepEnd;

   sem_t stop_sem;

   unsigned long wcet_us;

   unsigned long TASKLOADCOUNTER;

   unsigned long TASKLOADSTOPCOUNTER;

   ExecutionInterrupter *attached_EI;

public:

   TaskLoad();

   virtual ~TaskLoad();

   bool LoadsInterface(unsigned long _wcet_us);

   // void  trigerPAPIReading(int source);

   virtual bool runLoads(unsigned long _wcet_us);

   void setSuspendPoint();

   inline void initCheckCounter()
   {
      TASKLOADCOUNTER = 0;
   }

   // void suspend(const struct timespec& length);

   void setExecutionInterrupter(ExecutionInterrupter *ei);

   void stop();

   //void resume(int source);

   void dummy(void *array);

   void do_flops(int n);
};



#endif
