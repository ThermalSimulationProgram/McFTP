#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include <vector>
#include <string>
#include <semaphore.h>

#include "CMI.h"
#include "core/structdef.h"
#include "results/CPUUsage.h"
#include "utils/Enumerations.h"
#include "configuration/JobQueue.h"
#include "configuration/StateTable.h"
#include "../McFTPBuildConfig.h"

class Worker;
class Dispatcher;
class ThermalApproach;
class TempWatcher;
class Task;
class CPUUsage;
class PowerManager;
// class CMI;


class Processor {
protected:
   // This attribute indicates if the processor is initialized
   static bool initialized;

   // This attribute indicates if the processor is running
   static bool running;

   CMI *cmi;

   // number of used cores, should be set in the xml file
   int n_cores;

   // number of cpu cores in the hardware platform
   int n_cpus;

   // This vector stores the cpu core ids corresponding to each core
   std::vector <unsigned> worker_cpu;

   // This vector stores pointers to workers. Each worker represents
   // a core.
   std::vector <Worker *> workers;

   // Dispatchers release jobs to Processor
   std::vector <Dispatcher *> dispatchers;

   // JobQueue allTaskQueue;
   std::vector <JobQueue> allTaskQueues;

   std::vector <sem_t> taskqueue_sems;

   std::vector <sem_t> jobnumber_sems;

   std::vector <sem_t> papireading_sems;

   // ThermalApproach manages the execution of the workers
   ThermalApproach *thermal_approach;

   // TempWatcher periodically reads the temperatures of the cpu cores and
   // records them
   TempWatcher *tempwatcher;

   // PowerManager controls the power dissipation of the cores according to
   // StateTables given by thermal approaches
   PowerManager *powermanager;

   // auxiliary variable to set main thread priority
   struct sched_param param;

   // CPUUsage records cpu active and idle times, used to calculate cpu usage
   CPUUsage cpuUsageRecorder;

   // the number of created threads
   int thread_num;


   int _isAppendSaveFile;
public:

   static sem_t init_sem;

   static sem_t running_sem;

   // Constructor needs the cmi pointer
   explicit Processor(CMI *cmi);

   // Constructor needs the cmi pointer and append file flag
   explicit Processor(CMI *cmi, int);

   ~Processor();

   /************** Simulation interface functions ************/
   // prepare for simulation, initialize statistics, dispatcher, scheduler
   void initialize();

   // explicitly set the CPUs to which the workers are attached
   void setWorkerCPU(std::vector <unsigned>);

   // Start the simulation, the duration is loaded from Scratch class
   double simulate();

   // join other threads, wait them to finish
   void join_all();

   void setThermalApproachPeriod(unsigned long period_us);

   /************** Simulation functions ************/

   bool taskMigration(int source_id, int target_id);

   void newJob(Task *, _task_type);

   void finishedJob(Task *t);

   void insertJobToQueue(int queueId, Task *);

   void preemptCurrentJobOnCore(int coreId);

   Task *tryLoadJob(int workerId, struct timespec timeout);

   void updateStateTables(const std::vector <StateTable>& c);

   void lockTaskQueue(int coreId);

   void unlockTaskQueue(int coreId);

   void lockTaskQueues();

   void tryLockTaskQueues();

   void unlockTaskQueues();

   void lockAllWorkers();

   void unlockAllWorkers();

   // Interface function to get member 'initialized'
   static bool isInitialized();

   // Interface function to get member 'running'
   static bool isRunning();

   // This function collects all the information of the pipeline
   // in the simulation
   void getDynamicInfo(DynamicInfo& p);

   std::vector <int> getAllTaskIds();

   int getWorkerNumber();

   JobQueue *getJobQueuePointer(int queueId);

   void triggerAllPAPIReading(int source);

   void informPAPIReadingFinish(int workerId);

   int waitPAPIReading();

   void getPAPIValues(std::vector <std::vector <long long> >& values);
};



#endif
