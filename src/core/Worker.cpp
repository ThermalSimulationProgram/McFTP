#include "core/Worker.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "configuration/Scratch.h"
#include "utils/Enumerations.h"
#include "utils/Semaphores.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "results/Statistics.h"
#include "pthread/Priorities.h"
#include "configuration/Scratch.h"
#include "core/Processor.h"
#include "core/Task.h"
#include "core/ComponentsDefine.h"


using namespace std;

#define _INFO     0
#define _DEBUG    0



Worker::Worker(int _workerId, int _id) : Thread(_id)
{
   workerId    = _workerId;
   thread_type = _worker;
   current_job = NULL;
   state       = _active;

   sem_init(&job_sem, 0, 1);
   sem_init(&state_sem, 0, 1);
   // sem_init(&suspend_sem, 0, 0);
   // sem_init(&resume_sem, 0, 0);
   sem_init(&exetime_sem, 0, 1);
   sem_init(&stop_sem, 0, 0);


   latestSleep      = TimeUtil::Millis(0);
   latestExecuteJob = latestSleep;

   exeInterrupter.setWorker(this);
}

Worker::~Worker()
{
}

void Worker::join()
{
   // sem_post(&resume_sem);
   // cout << "worker " << id << " joining 1" << endl;
   sem_post(&state_sem);
   sem_post(&state_sem);
   sem_post(&job_sem);
   // cout << "worker " << id << " joining 2" << endl;
   exeInterrupter.resumeExecution(PROCESSOR);
   exeInterrupter.resumeExecution(PROCESSOR);
   exeInterrupter.resumeExecution(PROCESSOR);
   // cout << "worker " << id << " joining 3" << endl;
   stopCurrentJob();
   // cout << "worker " << id << " joining 4" << endl;
   join2();
}

void Worker::getCoreInfo(CoreInfo& cinfo)
{
   cinfo.workerId = workerId;
   sem_wait(&state_sem);
   cinfo.state           = state;
   cinfo.latestSleepTime = latestSleep;
   sem_post(&state_sem);

   cinfo.latestExecuteTime = getlatestExecuteJobTime();

   sem_wait(&job_sem);
   if (current_job != NULL)
   {
      cinfo.onGoJobId   = current_job->getId();
      cinfo.onGoJobType = current_job->getType();
   }
   else
   {
      cinfo.onGoJobId   = 0;
      cinfo.onGoJobType = singlecore;
   }
   sem_post(&job_sem);
}

Task *Worker::stopCurrentJob()
{
   Task *ret = NULL;

   // cout << "worker " << id << " stopCurrentJob 1" << endl;
   sem_wait(&job_sem);
   if (current_job != NULL)
   {
      ret = current_job;
      // wait the worker exits task->fire()
      // cout << "worker " << id << " stopCurrentJob 2" << endl;
      do
      {
         exeInterrupter.resumeExecution(PROCESSOR);
         current_job->stop();
      }while (sem_wait(&stop_sem) != 0);
      // cout << "worker " << id << " stopCurrentJob 3" << endl;
      current_job = NULL;
   }
   sem_post(&job_sem);
   return(ret);
}

void Worker::activate1()
{
   setPriority(Priorities::get_active_pr());
   sem_wait(&state_sem);
   state       = _active;
   latestSleep = TimeUtil::Millis(0);
   sem_post(&state_sem);
}

void Worker::activate(int source)
{
   activate1();

   exeInterrupter.resumeExecution(source);

   // sem_post(&resume_sem);
   // sem_wait(&job_sem);
   // if (current_job != NULL){
   //    current_job->resume(source);
   // }
   // sem_post(&job_sem);
}

void Worker::deactivate(const struct timespec &length)
{
   struct timespec now      = TimeUtil::getTime();
   struct timespec sleepEnd = now + length;

   sem_wait(&state_sem);
   latestSleep = now;
   state       = _sleep;
   sem_post(&state_sem);

   exeInterrupter.suspendExecution(sleepEnd);

   // sem_post(&suspend_sem);
   // sem_wait(&job_sem);
   // if (current_job != NULL){
   //    current_job->suspend(length);
   // }
   // sem_post(&job_sem);
}

bool Worker::isActive()
{
   bool ret;

   sem_wait(&state_sem);
   if (state == _sleep)
   {
      ret = false;
   }
   else
   {
      ret = true;
   }
   sem_post(&state_sem);
   return(ret);
}

void Worker::lockCurrentJob()
{
   sem_wait(&job_sem);
}

void Worker::unlockCurrentJob()
{
   sem_post(&job_sem);
}

void Worker::wrapper()
{
   struct timespec timeout = TimeUtil::Millis(200);

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (Scratch::isUsingSoftTempSensor())
   {
      PerformanceCounters::initializeThread();

      performanceCounter.initializeCounterSet();

      for (int i = 0; i < (int)Scratch::soft_sensors.size(); ++i)
      {
         performanceCounter.addCounter(Scratch::soft_sensors[i].counterName);
      }

      if (!performanceCounter.startAllCounters())
      {
         printf("Worker::wrapper: failed to start performance counters\n");
         exit(1);
      }
   }
        #endif

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "Worker: " << id << " waiting for initialization\n";
   Semaphores::print_sem.post_sem();
        #endif

   //Wait until the simulation is initialized
   sem_wait(&Processor::init_sem);

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "Worker: " << id << " waiting for simulation start\n";
   Semaphores::print_sem.post_sem();
        #endif

   ///wait for the simulation start
   sem_wait(&Processor::running_sem);

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "Worker: " << id << " begining execution \n";
   Semaphores::print_sem.post_sem();
        #endif

   Task *aux;
   exeInterrupter.informExecutionBegin();

   while (Processor::isRunning())
   {
      // cout << "worker: " << workerId << " loop 1 start" << endl;
      exeInterrupter.checkInterrupts();


      if (current_job == NULL)
      {
         do
         {
            aux = processor->tryLoadJob(workerId, timeout);
            exeInterrupter.checkInterrupts();
            // cout << "worker: loop 2 inside" << endl;
         }while (aux == NULL && Processor::isRunning());

         int value;
         sem_getvalue(&stop_sem, &value);
         for (int i = 0; i < value; ++i)
         {
            sem_trywait(&stop_sem);
         }
         if (aux != NULL)
         {
            sem_wait(&job_sem);
            current_job = aux;
            sem_post(&job_sem);
//
         }
      }
      else
      {
         // cout << "start execution tasks" << endl;
         current_job->setWorker(this);
         current_job->setLoadExecutionInterrupter(&exeInterrupter);

         sem_wait(&exetime_sem);
         latestExecuteJob = TimeUtil::getTime();
         sem_post(&exetime_sem);

         bool isFinished = current_job->fire(id);
         if (isFinished)
         {
            processor->finishedJob(current_job);
         }

         sem_wait(&job_sem);
         current_job = NULL;
         sem_post(&job_sem);
         // cout << "worker: " << workerId <<" finish execution tasks" << endl;
      }
   }

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (Scratch::isUsingSoftTempSensor())
   {
      performanceCounter.exitThread();
   }
        #endif

   sem_post(&stop_sem);
   Semaphores::print_sem.wait_sem();
   cout << "Worker: " << id << " is exiting wrapper \n";
   Semaphores::print_sem.post_sem();
}

void Worker::setProcessor(Processor *c)
{
   processor = c;
}

int Worker::getWorkerId()
{
   return(workerId);
}

struct timespec Worker::getLatestSleepTime()
{
   struct timespec ret;

   sem_wait(&state_sem);
   ret = latestSleep;
   sem_post(&state_sem);
   return(ret);
}

struct timespec Worker::getlatestExecuteJobTime()
{
   struct timespec ret;

   sem_wait(&exetime_sem);
   ret = latestExecuteJob;
   sem_post(&exetime_sem);
   return(ret);
}

int Worker::readPAPIValues()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   struct timespec start = TimeUtil::getTime();
   if (Scratch::isUsingSoftTempSensor())
   {
      performanceCounter.readAllValues();
      // papi_reading_time = TimeUtil::getTimeUSec()/1000;
      processor->informPAPIReadingFinish(workerId);
   }
   Statistics::addPerformanceCounterOverhead(
      TimeUtil::convert_us(TimeUtil::getTime() - start), workerId);
        #endif

   return(1);
}

void Worker::triggerPAPIReading(int source)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   performanceCounter.clearCounterValues();      // flush previous readings
   exeInterrupter.triggerPAPIReading(source);
        #endif
}

void Worker::getPAPIValues(std::vector <long long>& v)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (Scratch::isUsingSoftTempSensor())
   {
      v.clear();
      int numCounters = performanceCounter.getCounterNumber();
      for (int i = 0; i < numCounters; ++i)
      {
         v.push_back(performanceCounter.getCounterValue(i));
      }
   }
        #endif
}
