#ifndef _WORKER_H
#define _WORKER_H


#include <vector>
#include <deque>
#include <time.h>
#include <semaphore.h>

#include "../McFTPBuildConfig.h"

#include "configuration/JobQueue.h"
#include "pthread/Thread.h"
#include "core/structdef.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "core/ExecutionInterrupter.h"

#ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
#include "performance_counter/PerformanceCounters.h"
#endif

class Processor;

class Task;


class Worker : public Thread
{
protected:
   int workerId;

   enum _worker_state state;

   struct timespec latestSleep;

   struct timespec latestExecuteJob;

   Task *current_job;

   Processor *processor;

   // struct timespec sleepEnd;

   ExecutionInterrupter exeInterrupter;

   ///semaphore controls accessing state and latestSleep
   sem_t state_sem;

   sem_t exetime_sem;

   // sem_t suspend_sem;

   // sem_t resume_sem;

   sem_t job_sem;

   sem_t stop_sem;

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   PerformanceCounters performanceCounter;
        #endif

public:
   Worker(int, int);
   ~Worker();

   void join();

   Task *stopCurrentJob();

   void activate1();

   void activate(int source);

   void deactivate(const struct timespec &length);

   void wrapper();

   // lock current job. When the worker finishes current job,
   // it cannot load new job
   void lockCurrentJob();

   // unlock the job queue
   void unlockCurrentJob();

   void setProcessor(Processor *);

   void getCoreInfo(CoreInfo& cinfo);

   struct timespec getLatestSleepTime();

   struct timespec getlatestExecuteJobTime();

   bool isActive();

   int getWorkerId();

   int readPAPIValues();

   void getPAPIValues(std::vector <long long>& v);

   void triggerPAPIReading(int source);
};

#endif
