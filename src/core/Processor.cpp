#include "core/Processor.h"


#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

//components
#include "core/PowerManager.h"
#include "core/TempWatcher.h"
#include "core/Dispatcher.h"
#include "core/ThermalApproach.h"
#include "core/Worker.h"
#include "core/Task.h"
#include "dispatchers/Aperiodic.h"
#include "dispatchers/Periodic.h"
#include "dispatchers/PeriodicJitter.h"
#include "tasks/Pipelined.h"
#include "CMI.h"
#include "core/ComponentsDefine.h"


//parser
#include "utils/Parser.h"
#include "core/TaskArgument.h"
//utilities
#include "configuration/Scratch.h"
#include "utils/TimeUtil.h"
#include "utils/Semaphores.h"
#include "pthread/Priorities.h"
#include "results/Statistics.h"
#include "utils/Operators.h"
#include "utils/FileOperator.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"



using namespace std;

#define _DEBUG    0
#define _INFO     0


bool  Processor::initialized = false;
bool  Processor::running     = false;
sem_t Processor::init_sem;
sem_t Processor::running_sem;

// Constructor needs the xml file path
Processor::Processor(CMI *_cmi) : Processor(_cmi, 0)
{
}

// Constructor needs the xml file path
Processor::Processor(CMI *_cmi, int isAppendSaveFile) : cpuUsageRecorder()
{
   // give main thread the highest priority
   param.sched_priority = Priorities::get_main_pr();      //highest priority
   if ((sched_setscheduler(0, SCHED_FIFO, &param) != 0))
   {
      cout << "Run with root\n";
      pthread_exit(0);
   }


   cmi = _cmi;
   _isAppendSaveFile = isAppendSaveFile;
   thread_num        = 0;

   // compare stage number with CPU number
   int _n_used = Scratch::getNcores();

   n_cpus = (int)sysconf(_SC_NPROCESSORS_ONLN);
   if (_n_used <= 0 || _n_used > n_cpus)
   {
      cerr << "The number of cores on this machine is " << n_cpus << endl;
      cerr << "Invalid stage number, should be a positive integer which is no larger than n_cpus";
      pthread_exit(0);
   }
   n_cores = _n_used;

   vector <TaskArgument> allTaskData = Scratch::getTaskData();

   for (int i = 0; i < (int)allTaskData.size(); ++i)
   {
      switch (allTaskData[i]._periodicity)
      {
      case aperiodic: {
         Aperiodic *aux = new Aperiodic(100 + i);
         aux->setTaskData(allTaskData[i]);
         aux->setReleaseTime(allTaskData[i].release_time);
         aux->setProcessor(this);
         dispatchers.push_back((Dispatcher *)aux);
         break;
      }

      case periodic: {
         Periodic *aux = new Periodic(100 + i);
         aux->setTaskData(allTaskData[i]);
         aux->setPeriod(allTaskData[i].period);
         aux->setProcessor(this);
         dispatchers.push_back((Dispatcher *)aux);
         break;
      }

      case periodic_jitter: {
         PeriodicJitter *aux = new PeriodicJitter(100 + i);
         aux->setTaskData(allTaskData[i]);
         aux->setPeriod(allTaskData[i].period);
         aux->setJitter(allTaskData[i].jitter);
         aux->setProcessor(this);
         dispatchers.push_back((Dispatcher *)aux);
         break;
      }
      }
      ++thread_num;
   }


   thermal_approach = new ThermalApproach(cmi, 99, this, Scratch::getApproachName());
   ++thread_num;

   // create n_cores workers and attach it to default CPU
   for (int i = 0; i < n_cores; ++i)
   {
      Worker *t = new Worker(i, i);
      t->setProcessor(this);
      workers.push_back(t);
      worker_cpu.push_back(i);
      Statistics::initializeOnePCOverheadTrace();
      ++thread_num;
   }

   tempwatcher = new TempWatcher(100000, 98, Scratch::isUsingHardwareTempSensor(), Scratch::isUsingSoftTempSensor(), this);
   ++thread_num;

   powermanager = new PowerManager(200, workers);
   ++thread_num;

   for (int i = 0; i < n_cores; ++i)
   {
      sem_t aux;
      sem_init(&aux, 0, 2);           // at most two thread (a worker and a dispatcher) can access task queues
      taskqueue_sems.push_back(aux);

      sem_t aux2;
      sem_init(&aux2, 0, 0);
      jobnumber_sems.push_back(aux2);

      sem_t aux3;
      sem_init(&aux3, 0, 0);
      papireading_sems.push_back(aux3);

      JobQueue temp_q = JobQueue();
      allTaskQueues.push_back(temp_q);
   }

   sem_init(&init_sem, 0, 0);
   sem_init(&running_sem, 0, 0);
}

Processor::~Processor()
{
   delete thermal_approach;
   delete tempwatcher;
   delete powermanager;

   for (int i = 0; i < (int)dispatchers.size(); ++i)
   {
      delete dispatchers[i];
   }
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      delete workers[i];
   }
}

// prepare for simulation, initialize statistics, dispatcher, scheduler
void Processor::initialize()
{
   Statistics::initialize();

   for (int i = 0; i < (int)workers.size(); ++i)
   {
      workers[i]->setProcessor(this);
   }

   for (int i = 0; i < (int)dispatchers.size(); ++i)
   {
      dispatchers[i]->trigger();
   }

   powermanager->trigger();

   thermal_approach->trigger();
   thermal_approach->setCPU(n_cpus - 1);
   thermal_approach->activate();

   tempwatcher->trigger();

   // Main thread waits for all threads initialized, especially the scheduler
   Semaphores::rtcinit_sem.wait_sem();
   // all threads initialized
   initialized = true;
   for (int i = 0; i < thread_num; ++i)
   {
      sem_post(&init_sem);
   }
}

// explicitly set the CPUs to which the works are attached
void Processor::setWorkerCPU(vector <unsigned> order)
{
   if (order.size() != (unsigned)n_cores)
   {
      cout << "Processor::setWorkerCPU: invalid input\n";
      return;
   }
   for (unsigned i = 0; i < order.size(); ++i)
   {
      if (order[i] < 0 || order[i] >= (unsigned)n_cpus)
      {
         cout << "Processor::setWorkerCPU: invalid input\n";
         return;
      }
   }
   worker_cpu = order;
}

// Start the simulation, the duration is loaded from Scratch class
double Processor::simulate()
{
   // activate all the threads and attach them to their CPUs.
   for (unsigned i = 0; i < workers.size(); ++i)
   {
      workers[i]->trigger();
      workers[i]->setCPU(worker_cpu[i]);
      workers[i]->activate1();
   }

   for (int i = 0; i < (int)dispatchers.size(); ++i)
   {
      dispatchers[i]->setCPU(n_cpus - 2);
      dispatchers[i]->activate();
   }

   tempwatcher->activate();
   if (workers.size() == 1)
   {
      tempwatcher->setCPU(n_cpus - worker_cpu[0] - 1);
   }
   else
   {
      tempwatcher->setCPU(n_cpus - 2);
   }

   powermanager->setCPU(n_cpus - 2);
   powermanager->activate();

   Statistics::enable();

   cout << "start simulation" << endl;

   // simulation starts now
   cpuUsageRecorder.startLoggingCPU();
   // record start time and send it to TimedRunnable objects
   struct timespec rem;
   struct timespec duration = TimeUtil::Micros(Scratch::getDuration());

   running = true;

   for (int i = 0; i < thread_num; ++i)
   {
      sem_post(&running_sem);
   }


   TimeUtil::setOffset();
   // main thread sleeps for duration time length
   nanosleep(&duration, &rem);
   running = false;

   // unblock waiting threads
   for (int i = 0; i < n_cores; ++i)
   {
      sem_post(&jobnumber_sems[i]);
      sem_post(&jobnumber_sems[i]);
      sem_post(&taskqueue_sems[i]);
      sem_post(&taskqueue_sems[i]);
   }

   // join other threads, wait them to finish
   join_all();
   enum _thread_type main_type = _main;

   Statistics::addRuntime(main_type, 1000, TimeUtil::getTime(_relative));

   std::vector <TaskArgument> allTaskData = Scratch::getTaskData();
   int task_num = allTaskData.size();

   for (int i = 0; i < (int)workers.size(); ++i)
   {
      struct timespec this_worker_job_time = TimeUtil::Micros(0);
      int             this_worker_id       = workers[i]->getWorkerId();

      for (int j = 0; j < task_num; ++j)
      {
         int counter = Statistics::getJobLogNumber(this_worker_id,
                                                   allTaskData[j].taskId);
         this_worker_job_time = this_worker_job_time + counter * allTaskData[j].wcets[this_worker_id];
      }

      Statistics::addRuntime(_worker, workers[i]->getId(), this_worker_job_time);
   }

   // return the average temperature
   cout << "cpuUsageRecorder..." << endl;
   cpuUsageRecorder.endLoggingCPU();

   Statistics::toFile(Scratch::getName());

   return(0);
}

void Processor::join_all()
{
   for (int i = 0; i < (int)dispatchers.size(); ++i)
   {
      if (dispatchers[i] != NULL)
      {
         dispatchers[i]->join();
      }
   }
   cout << "processor is joining all threads" << endl;
   thermal_approach->join();

   tempwatcher->join();

   powermanager->join();

   Worker *t;

   for (unsigned i = 0; i < workers.size(); i++)
   {
      t = workers[i];
      if (t != NULL)
      {
         cout << "processor is joining worker " << i << endl;
         t->join();
      }
   }


   // #if _INFO==1
   cout << "Joined all!\n";
   // #endif
}

void Processor::newJob(Task *t, _task_type type)
{
   int id;

   if (type == pipelined)
   {
      id = 0;
   }
   id = cmi->getNewJobTargetCore(t, type);
   // cout << "Processor::newJob: task Id: " << t->getTaskId() <<
   // "Job id " << t->getId() << endl;
   insertJobToQueue(id, t);
   // cout << "Processor::newJob: inserted " << endl;
}

void Processor::insertJobToQueue(int queueId, Task *newTask)
{
   sem_wait(&taskqueue_sems[queueId]);
   allTaskQueues[queueId].insertJob(newTask);
   sem_post(&jobnumber_sems[queueId]);
   sem_post(&taskqueue_sems[queueId]);
}

// This function is called by the end stage to announce a job is finished
void Processor::finishedJob(Task *t)
{
   if (t->isFinished())
   {
      cmi->finishedJob(t);
   }
   else
   {
      if (t->getType() == pipelined)
      {
         // cout << "CMI::finishedJob with wcet: " <<t->getWCET(0) << ". insert to next stage at time: "
         // << getCurrentSimTime_ms() << endl;
         Pipelined *b = (Pipelined *)t;
         insertJobToQueue(b->getNextCoreId(), t);
      }
   }
}

Task *Processor::tryLoadJob(int workerId, struct timespec timeout)
{
   Task *ret = NULL;

   struct timespec waitEnd = TimeUtil::getTime() + timeout;

   // wait at most timeout time units if the task queue is blocked
   if (sem_timedwait(&taskqueue_sems[workerId], &waitEnd) == 0)
   {
      // if we recieve the signal

      // wait if there is no job in the queue
      if (sem_timedwait(&jobnumber_sems[workerId], &waitEnd) == 0)
      {
         ret = allTaskQueues[workerId].pop_front();
      }
      // unlock the task queue
      sem_post(&taskqueue_sems[workerId]);
   }

   return(ret);
}

void Processor::getDynamicInfo(DynamicInfo& dinfo)
{
   while ((int)dinfo.coreinfos.size() < n_cores)
   {
      CoreInfo tmp;
      dinfo.coreinfos.push_back(tmp);
   }

   dinfo.hardwareTemperature = tempwatcher->getCurHardwareTemp();
   dinfo.softwareTemperature = tempwatcher->getCurSoftwareTemp();
   dinfo.numstages           = n_cores;

   // get current time from simulation start, unit ms
   double curTime = (double)TimeUtil::getTimeMSec();

   dinfo.currentTime = curTime;

   for (int i = 0; i < n_cores; ++i)
   {
      workers[i]->getCoreInfo(dinfo.coreinfos[i]);
   }

   for (int i = 0; i < n_cores; ++i)
   {
      dinfo.tasksInQueue.push_back(allTaskQueues[i].peekAllTasks());
   }
}

void Processor::lockTaskQueue(int coreId)
{
   sem_wait(&taskqueue_sems[coreId]);
}

void Processor::unlockTaskQueue(int coreId)
{
   sem_post(&taskqueue_sems[coreId]);
}

void Processor::lockTaskQueues()
{
   for (int i = 0; i < n_cores; ++i)
   {
      sem_wait(&taskqueue_sems[i]);
   }
}

void Processor::tryLockTaskQueues()
{
   for (int i = 0; i < n_cores; ++i)
   {
      sem_trywait(&taskqueue_sems[i]);
   }
}

void Processor::unlockTaskQueues()
{
   for (int i = 0; i < n_cores; ++i)
   {
      sem_post(&taskqueue_sems[i]);
   }
}

void Processor::lockAllWorkers()
{
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      workers[i]->lockCurrentJob();
   }
}

void Processor::unlockAllWorkers()
{
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      workers[i]->unlockCurrentJob();
   }
}

// Interface function to get member 'initialized'
bool Processor::isInitialized()
{
   return(initialized);
}

// Interface function to get member 'simulating'
bool Processor::isRunning()
{
   return(running);
}

bool Processor::taskMigration(int source_id, int target_id)
{
   bool  ret        = false;
   Task *target_job = workers[source_id]->stopCurrentJob();

   if (target_job != NULL)
   {
      insertJobToQueue(target_id, target_job);
      ret = true;
   }
   return(ret);
}

void Processor::updateStateTables(const std::vector <StateTable>& stables)
{
   if (stables.size() == 0)
   {
      return;
   }
   if ((int)stables.size() == n_cores)
   {
      powermanager->setStateTables(stables);
   }
   else
   {
      for (int i = 0; i < (int)stables.size(); ++i)
      {
         powermanager->setStateTable(stables[i]);
      }
   }
}

void Processor::setThermalApproachPeriod(unsigned long period_us)
{
   thermal_approach->setPeriod(period_us);
}

vector <int> Processor::getAllTaskIds()
{
   vector <int> taskIds;

   for (int i = 0; i < (int)dispatchers.size(); ++i)
   {
      taskIds.push_back(dispatchers[i]->getTaskId());
   }
   return(taskIds);
}

int Processor::getWorkerNumber()
{
   return((int)workers.size());
}

JobQueue *Processor::getJobQueuePointer(int queueId)
{
   if (queueId >= 0 && queueId < n_cores)
   {
      return(&allTaskQueues[queueId]);
   }
   else
   {
      return(NULL);
   }
}

void Processor::preemptCurrentJobOnCore(int coreId)
{
   int   r = sem_trywait(&taskqueue_sems[coreId]);
   Task *t = workers[coreId]->stopCurrentJob();

   allTaskQueues[coreId].insertJobAt(1, t);
   if (r == 0)
   {
      sem_post(&taskqueue_sems[coreId]);
   }
}

void Processor::triggerAllPAPIReading(int source)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   // cout << "processor trigger all papi" << endl;
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      workers[i]->triggerPAPIReading(source);
      // cout << "papi " << i << " triggered" << endl;
   }
        #endif
}

void Processor::informPAPIReadingFinish(int workerId)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (workerId < 0 || workerId >= n_cores)
   {
      return;
   }
   sem_post(&papireading_sems[workerId]);
        #endif
}

int Processor::waitPAPIReading()
{
   int ret = 0;

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   struct timespec waitEnd = TimeUtil::getTime() + TimeUtil::Millis(20);
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      ret += sem_timedwait(&papireading_sems[i], &waitEnd);
   }
        #endif
   return(ret);
}

void Processor::getPAPIValues(vector <vector <long long> >& values)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   values.clear();
   for (int i = 0; i < (int)workers.size(); ++i)
   {
      vector <long long> temp;
      workers[i]->getPAPIValues(temp);
      values.push_back(temp);
   }
        #endif
}
