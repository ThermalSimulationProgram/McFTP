#ifndef CMI_H
#define CMI_H

#include <vector>
#include <string>
#include <semaphore.h>

#include "core/Task.h"
#include "configuration/JobQueue.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "core/structdef.h"
#include "configuration/StateTable.h"

#include "CMIFunctionPointerDefs.h"


class Processor;

class CMI {
private:
   std::string xml_path;

   int workerNumber;

   // pointer to the processor object, containing all necessary entities
   Processor *processor;

   // The offline approach will be called only once at the beginning of the experiment
   offline_thermal_approach offlineApproach;

   // the online thermal approach, default NULL.
   online_thermal_approach onlineApproach;

   // function pointer to the function allocating new Jobs.
   // The function will ba called every time a new job is created.
   online_task_allocator taskAllocator;

   // This is the default task allocator
   // syntax: staticTaskAllocator[TASKID] returns the id of the worker on which
   // the new created job whose taskid is TASKID is goind to be executed.
   // users can set it before running the experiment
   std::vector <int> staticTaskAllocator;

   // In the case of using soft temperature sensor,
   // mcftp will call the default linear soft temperature calculator to get the temperature
   // for each given performance counter, if this variable is set to NULL
   // Otherwise, this function will be called to obtain the temperatures according to
   // the values of the performance counters
   user_defined_soft_temperature_sensor softSensor;

   // semaphore to control the access to staticTaskAllocator
   sem_t task_allocator_sem;
protected:

   bool checkCoreId(int id);

public:
   // Constructor needs the path to the xml file
   CMI(std::string);

   CMI(std::string, int isAppendSaveFile);

   // Destructor
   ~CMI();

   /*********Call Below Functions Before Starting A Experiment********/
   // Call Below Functions Before Starting A Experiment to customize
   // the experiment

   // Set the offline thermal approach, this function will be executed at the
   // beginning of the experiment
   void setOfflineThermalApproach(offline_thermal_approach _approach);

   // Set the online thermal approach, this function will be executed periodically
   // during the experiment
   void setOnlineThermalApproach(online_thermal_approach _approach);

   // Set the period of running the online approach
   void setOnlineThermalApproachPeriod(unsigned long period_us);

   // Statically link the jobs of A task to a core. Note all such static settings
   // are invalid if an online task allocator is given
   void setTaskRunningCore(int _taskId, int _coreId);

   // Set the online task allocator. The allocator will be called every time
   // a new task is created to determine on which core the new task should be executed
   void setOnlineTaskAllocator(online_task_allocator _allocator);

   // turn on the soft temperature sensor feature
   void enableSoftTemperatureSensor();

   // The user can customize the soft temperature sensor by passing the
   // function to this method.
   void setSoftTemperatureSensor(user_defined_soft_temperature_sensor _sensor);

   // Get the task-indexes of all tasks. The task-index only specifies the type
   // of the task.
   std::vector <int> getAllTaskIds();


   // After setting all the configurations, call this method to initialize all
   // the components needed for the experiment
   void initializeComponents();

   // Call this function to start the experiment
   void startRunning();


   /*********CMI Interface Functions********/
   // Below functions should be called in the online thermal approach during
   // experiments

   // Lock all jobs existing in the processor. Pause the experiment.
   // No job can inserted into any job queue,
   // No job can be poped from its queue, already finished jobs in all the cores will
   // still keep in the core.
   // Call this function when necessary, i.e., if for deterministic manipulation
   // on the processor.
   void lockAllJobs();

   // Unlock all jobs. The experiment continues.
   void unlockAllJobs();

   // Move the task currently running on core source_core_id to the
   // job queue of core target_core_id
   void taskMigrate(int source_core_id, int target_core_id);

   // Advances the task at the position taskPosition in job queue with id queueId
   // by n jobs position. When n > taskPosition, the task is moved to the front
   void advanceJobInQueue(int queueId, int taskPosition, int n);

   // Recede the task at the position taskPosition in job queue with id queueId
   // by n jobs position. When n > QueueSize - taskPosition, the task id moved
   // to the back.
   void recedeJobInQueue(int queueId, int taskPosition, int n);

   // Preempt the currently running job on core coreId with the job at the
   // front of the job queue.
   void preemptCurrentJobOnCore(int coreId);

   // Move a task at
   // the position source_task_position in job queue with id source_queue_id
   // to
   // the position target_task_position in job queue with id target_queue_id
   void moveJobToAnotherQueue(int source_queue_id, int source_task_position,
                              int target_queue_id, int target_task_position);

   // Get the current time relative to the start of experiment in unit Microsecond
   unsigned long getRelativeTimeUsec();

   // Get the number of used cores
   int getCoreNumber();

   void displayAllQueues();

   // display the information of all the perfromance counters supported by this machine
   void displayPerformanceCounterInfo();


   /*********Basic Functions Automatically Called During Experiment********/

   // This function is called by the dispatcher to get the core id where a new job is released
   int getNewJobTargetCore(Task *, _task_type);

   // This function is called by the worker when a job is finished.
   // You can modify this function to customize the action
   void finishedJob(Task *);

   // Below functions are called by the thermal approach thread
   // This function collects all the information of the processor
   // in the simulation
   void getDynamicInfo(DynamicInfo& p);

   bool isOfflineApproach();

   // check whether an online approach is given
   bool isOnlineApproach();

   // run the offline approach, called by the thermal approach thread
   void runOfflineApproach(std::vector <StateTable>& c);

   // run the online approach, called by the thermal approach thread
   void runOnlineApproach(const DynamicInfo& p, std::vector <StateTable>& c);


   /*********Debug Functions********/
   void printAllTaskInfo();
};



#endif
