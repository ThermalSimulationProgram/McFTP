#include "CMI.h"

#include <iostream>

#include "core/Processor.h"

#include "configuration/Scratch.h"
#include "results/Statistics.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"
//parser
#include "utils/Parser.h"
#include "utils/exceptions.h"

#include "performance_counter/PerformanceCounters.h"


using namespace std;


void PeriodicThermalManagement(CMI *cmi, std::vector <StateTable>& c)
{
   vector <struct timespec> tons  = Scratch::getPBOOTons();
   vector <struct timespec> toffs = Scratch::getPBOOToffs();

   c.clear();

   for (int i = 0; i < (int)tons.size(); ++i)
   {
      StateTable temp = StateTable(i);
      temp.pushState(3400, TimeUtil::convert_us(tons[i]));
      temp.pushState(0, TimeUtil::convert_us(toffs[i]));
      c.push_back(temp);
   }
}

void StateTableManagement(CMI *cmi, std::vector <StateTable>& c)
{
   c.clear();

   for (int i = 0; i < cmi->getCoreNumber(); ++i)
   {
      StateTable temp = StateTable(i);


      temp.pushState(3400000, 100000000);
      temp.pushState(800000, 100000000);
      temp.pushState(2100000, 100000000);
      temp.pushState(3000000, 100000000);
      temp.pushState(0, 300000000);
      c.push_back(temp);
   }
}

CMI::CMI(string _xml_path) : CMI(xml_path, 0)
{
}

CMI::CMI(string _xml_path, int isAppendSaveFile)
{
   xml_path = _xml_path;
   Parser *p = new Parser(xml_path);

   try{
      p->parseFile();
   }
   catch (XmlParseException& e) {
      cout << "Processor::Processor: failed to parse given xml file!\n";
      cout << "The reason is: " << e.what() << endl;
      delete p;
      exit(1);
   }



   // if (p->parseFile() != 0){
   //    cerr << "Processor::Processor: failed to parse given xml file!\n";
   //    exit(1);
   // }
   delete p;

   Scratch::isAppendSaveFile = isAppendSaveFile;

   // default: no offline thermal approach
   offlineApproach = NULL;

   // default: no online thermal approach
   onlineApproach = NULL;

   // default: no dynamic task allocator
   taskAllocator = NULL;

   //default: the builtin linear soft temperature calculator
   softSensor = NULL;

   /** create the static task allocator **/
   // firstly, get all the task indexes
   vector <int> allTaskIds = Scratch::getAllTaskIds();
   // then, find the maximal task index, use it as the size of the static task allocator
   int maxTaskId = maxElement(allTaskIds);

   // create the static task allocator, default value -1 means invalid task index
   staticTaskAllocator = vector <int>(maxTaskId + 1, -1);
   // set default task allocator
   // The tasks are allocated in a circular manner
   workerNumber = Scratch::getNcores();
   // The workers are indexed from 0 to workerNumber-1
   int workerId = 0;      //
   vector <TaskArgument> xmlTaskData = Scratch::getTaskData();

   for (int i = 0; i < (int)allTaskIds.size(); ++i)
   {
      // invalid value or user does not give the core id in xml
      if (xmlTaskData[i].default_coreId < 0 ||
          xmlTaskData[i].default_coreId >= workerNumber)
      {
         staticTaskAllocator[allTaskIds[i]] = workerId;
      }
      else             // valid value
      {
         staticTaskAllocator[allTaskIds[i]] = xmlTaskData[i].default_coreId;
      }

      ++workerId;
      if (workerId >= workerNumber)
      {
         // switch back to the first worker
         workerId = 0;
      }
   }

   sem_init(&task_allocator_sem, 0, 1);
}

CMI::~CMI()
{
   delete processor;
}

/*********Call Below Functions Before Starting A Experiment********/
// Call Below Functions Before Starting A Experiment to customize
// the experiment

// Set the offline thermal approach, this function will be executed at the
// beginning of the experiment
void CMI::setOfflineThermalApproach(offline_thermal_approach _approach)
{
   offlineApproach = _approach;
}

// Set the online thermal approach, this function will be executed periodically
// during the experiment
void CMI::setOnlineThermalApproach(online_thermal_approach _approach)
{
   onlineApproach = _approach;
}

// Set the period of running the online approach
void CMI::setOnlineThermalApproachPeriod(unsigned long period_us)
{
   processor->setThermalApproachPeriod(period_us);
}

// Statically link the jobs of A task to a core. Note all such static settings
// are invalid if an online task allocator is given
void CMI::setTaskRunningCore(int _taskId, int _workerId)
{
   if ((_taskId < 0) || (_taskId >= (int)staticTaskAllocator.size()) ||
       staticTaskAllocator[_taskId] < 0)
   {
      cout << "CMI::setTaskRunningCore: error! Input taskId: " << _taskId
           << " does not exist" << endl;
      exit(1);
   }
   if (!checkCoreId(_workerId))
   {
      cout << "CMI::setTaskRunningCore: error! Input workerId: " << _workerId
           << " does not exist" << endl;
      exit(1);
   }
   staticTaskAllocator[_taskId] = _workerId;
}

// Set the online task allocator. The allocator will be called every time
// a new task is created to determine on which core the new task should be executed
void CMI::setOnlineTaskAllocator(online_task_allocator _allocator)
{
   taskAllocator = _allocator;
}

void CMI::enableSoftTemperatureSensor()
{
   Scratch::useSoftwareTempSensor = true;
}

// The user can customize the soft temperature sensor by passing the
// function to this method.
void CMI::setSoftTemperatureSensor(user_defined_soft_temperature_sensor _sensor)
{
   if (_sensor == NULL)
   {
      cout << "CMI: setSoftTemperatureSensor: input function pointer is NULL! " << endl
           << "Default linear soft temperature sensor will be used!" << endl;
   }
   Scratch::softSensor = _sensor;
   softSensor          = _sensor;
}

// Get the task-indexes of all tasks. The task-index only specifies the type
// of the task.
vector <int> CMI::getAllTaskIds()
{
   return(processor->getAllTaskIds());
}

// After setting all the configurations, call this method to initialize all
// the components needed for the experiment
void CMI::initializeComponents()
{
   if (onlineApproach == NULL && offlineApproach == NULL)
   {
      vector <struct timespec> tons = Scratch::getPBOOTons();
      if (tons.size() > 0)
      {
         offlineApproach = PeriodicThermalManagement;
      }
   }
   // construct the prcessor, including all workers, dispatchers,
   // power manager, temperature watcher, etc.
   processor = new Processor(this, Scratch::isAppendSaveFile);
}

// Call this function to start the experiment
void CMI::startRunning()
{
   processor->initialize();
   processor->simulate();
}

/*********CMI Interface Functions********/
// Below functions should be called in the online thermal approach during
// experiments

// Lock all jobs existing in the processor. Pause the experiment.
// No job can inserted into any job queue,
// No job can be poped from its queue, already finished jobs in all the cores will
// still keep in the core.
// Call this function when necessary, i.e., if for deterministic manipulation
// on the processor.
void CMI::lockAllJobs()
{
   processor->lockTaskQueues();
   processor->lockAllWorkers();
}

// Unlock all jobs. The experiment continues.
void CMI::unlockAllJobs()
{
   processor->unlockTaskQueues();
   processor->unlockAllWorkers();
}

// Move the task currently running on core source_core_id to the
// job queue of core target_core_id
void CMI::taskMigrate(int source_core_id, int target_core_id)
{
   if (source_core_id == target_core_id)
   {
      return;
   }
   if (!checkCoreId(source_core_id) || !checkCoreId(target_core_id))
   {
      return;
   }
   processor->taskMigration(source_core_id, target_core_id);
}

// Advances the job at the position taskPosition in job queue with id queueId
// by n jobs position. When n > taskPosition, the task is moved to the front
void CMI::advanceJobInQueue(int queueId, int taskPosition, int n)
{
   if (n == 0)
   {
      return;
   }
   JobQueue *p = processor->getJobQueuePointer(queueId);

   if (p != NULL)
   {
      int newPosition = taskPosition - n;
      if (newPosition < 0)
      {
         newPosition = 0;
      }
      Task *t = p->pop_at(taskPosition);
      if (t != NULL)
      {
         p->insertJobAt(newPosition, t);
      }
   }
}

// Recede the job at the position taskPosition in job queue with id queueId
// by n jobs position. When n > QueueSize - taskPosition, the task id moved
// to the back.
void CMI::recedeJobInQueue(int queueId, int taskPosition, int n)
{
   advanceJobInQueue(queueId, taskPosition, -n);
}

// Preempt the currently running job on core coreId with the job at the
// front of the job queue.
void CMI::preemptCurrentJobOnCore(int coreId)
{
   processor->preemptCurrentJobOnCore(coreId);
}

// Move a job at
// the position source_task_position in job queue with id source_queue_id
// to
// the position target_task_position in job queue with id target_queue_id
void CMI::moveJobToAnotherQueue(int source_queue_id, int source_task_position,
                                int target_queue_id, int target_task_position)
{
   if (!checkCoreId(source_queue_id) || !checkCoreId(target_queue_id))
   {
      return;
   }
   if (source_task_position < 0 || target_task_position < 0)
   {
      return;
   }
   JobQueue *p1 = processor->getJobQueuePointer(source_queue_id);
   JobQueue *p2 = processor->getJobQueuePointer(target_queue_id);

   if (p1 != NULL && p2 != NULL)
   {
      Task *t = p1->pop_at(source_task_position);
      if (t != NULL)
      {
         p2->insertJobAt(target_task_position, t);
      }
   }
}

// Get the current time relative to the start of experiment in unit Microsecond
unsigned long CMI::getRelativeTimeUsec()
{
   if (processor->isRunning())
   {
      return(TimeUtil::getTimeUSec());
   }
   else
   {
      return(0);
   }
}

// Get the number of used cores
int CMI::getCoreNumber()
{
   return(workerNumber);
}

void CMI::displayAllQueues()
{
   for (int i = 0; i < workerNumber; ++i)
   {
      JobQueue *p = processor->getJobQueuePointer(i);
      if (p != NULL)
      {
         cout << "JobQueue: " << i << "    ";
         p->print();
         cout << endl << "****************************************" << endl;
      }
   }
}

void CMI::displayPerformanceCounterInfo()
{
   // PerformanceCounters::printCounterInfo();
}

/*********Basic Functions Automatically Called During Experiment********/

// This function is called by the dispatcher to get the core id where a new job is released
int CMI::getNewJobTargetCore(Task *t, _task_type type)
{
   if (taskAllocator == NULL)
   {
      int ret;
      sem_wait(&task_allocator_sem);
      ret = staticTaskAllocator[t->getTaskId()];
      sem_post(&task_allocator_sem);

      return(ret);
   }
   else
   {
      int coreId = taskAllocator(this, t->getTaskId());
      if (coreId < 0 || coreId >= workerNumber)
      {
         cout << "CMI::getNewJobTargetCore: Given dynamic task allocator returns an invalid core index. "
              << "It will be modified to default 0! " << endl;
         coreId = 0;
      }
      return(coreId);
   }
}

// This function is called by the work when a job is finished.
// You can modify this function to customize the action
void CMI::finishedJob(Task *_task)
{
   cout << "Job with taskid: " << _task->getTaskId() << " with job id "
        << _task->getId() << " finishes at time: " << getRelativeTimeUsec() / 1000
        << " ms on core with id: " << _task->getFinishCoreId() << endl;
}

// Below functions are called by the thermal approach thread
// This function collects all the information of the processor
// in the simulation
void CMI::getDynamicInfo(DynamicInfo& p)
{
   processor->getDynamicInfo(p);
}

bool CMI::isOfflineApproach()
{
   return(offlineApproach != NULL);
}

bool CMI::isOnlineApproach()
{
   return(onlineApproach != NULL);
}

// run the offline approach, called by the thermal approach thread
void CMI::runOfflineApproach(std::vector <StateTable>& statetables)
{
   if (offlineApproach != NULL)
   {
      offlineApproach(this, statetables);
   }
}

// run the online approach, called by the thermal approach thread
void CMI::runOnlineApproach(const DynamicInfo& dinfo, std::vector <StateTable>& statetables)
{
   if (onlineApproach != NULL)
   {
      onlineApproach(this, dinfo, statetables);
   }
}

/** Helper Functions**/
bool CMI::checkCoreId(int id)
{
   if (id < 0 || id >= workerNumber)
   {
      return(false);
   }
   else
   {
      return(true);
   }
}

void CMI::printAllTaskInfo()
{
   Scratch::printAllTaskInfo();
}
