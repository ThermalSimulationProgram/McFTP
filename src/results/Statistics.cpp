#include "results/Statistics.h"

#include <fstream>
#include <iostream>

#include "utils/TimeUtil.h"
#include "utils/Semaphores.h"
#include "utils/Operators.h"
#include "utils/vectormath.h"
#include "configuration/Scratch.h"
#include "utils/FileOperator.h"
#include "utils/utils.h"
#include "../McFTPBuildConfig.h"

#define _TRACING_ENABLED    1

using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** VARIABLES ***********/

vector <vector <double> > Statistics::hardwareTemperatures;

vector <vector <double> > Statistics::softwareTemperatures;

vector <vector <long long> > Statistics::performanceCounterValues;

vector <double> Statistics::hardwareReadingOverheads;

vector <double> Statistics::softwareReadingOverheads;

vector <vector <double> > Statistics::performanceCounterReadingOverheads;


///This vector holds all of the simulation's runtime statistics
vector <Runtime> Statistics::runtimes;

///This vector holds alll of the time traces
vector <Trace> Statistics::traces;

vector <JobLog> Statistics::joblogs;

///This vector holds all of the missed deadlines
vector <MissedDeadline> Statistics::missedDeadlines;

//This vector holds the thread id's of all workers
vector <unsigned int> Statistics::workerId;

///This variable holds the state. If state==1, calling the ``add'' functions will add to statistics vectors. Otherwise, traces will not be saved
int Statistics::state = 0;

sem_t Statistics::runtime_sem;

///Semaphore to protect writing to the trace vector
sem_t Statistics::trace_sem;

///Semaphore to protect writing to the missedDealine vector
sem_t Statistics::deadline_sem;

sem_t Statistics::joblog_sem;

/*********** MEMBER FUNCTIONS ***********/

void Statistics::addHardwareTemperature(const std::vector <double>& t)
{
   hardwareTemperatures.push_back(t);
}

void Statistics::addSoftwareTemperature(const std::vector <double>& t)
{
   softwareTemperatures.push_back(t);
}

void Statistics::addPerformanceCounterValue(const std::vector <long long>& v)
{
   performanceCounterValues.push_back(v);
}

void Statistics::addHardwareReadingOverhead(double t)
{
   hardwareReadingOverheads.push_back(t);
}

void Statistics::addSoftwareReadingOverhead(double t)
{
   softwareReadingOverheads.push_back(t);
}

void Statistics::initializeOnePCOverheadTrace()
{
   vector <double> temp;

   performanceCounterReadingOverheads.push_back(temp);
}

void Statistics::addPerformanceCounterOverhead(double v, int id)
{
   if (id < 0 || id > (int)performanceCounterReadingOverheads.size())
   {
      return;
   }
   performanceCounterReadingOverheads[id].push_back(v);
}

///This function adds a missed deadline trace to the vector
void Statistics::addMissedDeadline(unsigned int t_id, unsigned long arrival_time, unsigned long deadline,
                                   unsigned long finisht_time)
{
    #if _TRACING_ENABLED == 1
   if (state == 1)
   {
      MissedDeadline md;


      sem_wait(&deadline_sem);
      md.setMissedDeadline(t_id, arrival_time, deadline, finisht_time);
      missedDeadlines.push_back(md);
      sem_post(&deadline_sem);
   }
    #endif
}

std::vector <string> Statistics::getAllMissedDeadline()
{
   vector <string> ret;

   for (int i = 0; i < (int)missedDeadlines.size(); ++i)
   {
      ret.push_back(missedDeadlines[i].toString());
   }
   return(ret);
}

///This function adds a runtime to the vector
void Statistics::addRuntime(enum _thread_type type, unsigned int id, struct timespec _rt)
{
   Runtime rt;

   // int v = -1;
   // Semaphores::print_sem.wait_sem();
   // sem_getvalue(&runtime_sem, &v);
   // cout << "Statistics::addRuntime:waiting for the semaphore, value is " << v << endl;
   // Semaphores::print_sem.post_sem();

   sem_wait(&runtime_sem);
   rt.setRuntime(type, id, _rt);
   runtimes.push_back(rt);
   sem_post(&runtime_sem);

   // Semaphores::print_sem.wait_sem();
   // sem_getvalue(&runtime_sem, &v);
   // cout << "Statistics::addRuntime:already posted the semaphore, value is " << v << endl;
   // Semaphores::print_sem.post_sem();
}

void Statistics::addJobLog(int workerid, int taskid)
{
   JobLog jl = JobLog(workerid, taskid);

   sem_wait(&joblog_sem);
   joblogs.push_back(jl);
   sem_post(&joblog_sem);
}

int Statistics::getJobLogNumber(int workerid, int taskid)
{
   int counter = 0;

   sem_wait(&joblog_sem);
   for (int i = 0; i < (int)joblogs.size(); ++i)
   {
      if (joblogs[i].isMatch(workerid, taskid))
      {
         counter++;
      }
   }
   sem_post(&joblog_sem);
   return(counter);
}

///This function adds a trace to the vector
void Statistics::addTrace(enum _thread_type type, int t_id, enum _task_action act)
{
    #if _TRACING_ENABLED == 1
   if (state == 1)
   {
      Trace           t;
      struct timespec aux;


      sem_wait(&trace_sem);
      aux = TimeUtil::getTime(_relative);
      // aux = aux -tstart;
      t.setTrace(aux, type, t_id, act);
      traces.push_back(t);
      sem_post(&trace_sem);
   }
    #endif
}

///This function should be called by the Worker constructor to 'register' its id
void Statistics::addWorkerId(unsigned int id)
{
    #if _INFO == 1
   cout << "Adding Worker ID: " << id << endl;
    #endif

   workerId.push_back(id);
}

///This function enables the collection of statistics
void Statistics::enable()
{
   state = 1;
}

///This function disable the collection of statistics
void Statistics::disable()
{
   state = 0;
}

///This function initiates all of the variables
void Statistics::initialize()
{
   runtimes.reserve(N_THREADS);
   traces.reserve(N_TRACES);
   missedDeadlines.reserve(N_DEADLINES);
   joblogs.reserve(N_JOBLOGS);

   //Initialize semaphores to 1, so it acts like a mutex
   sem_init(&trace_sem, 0, 1);
   sem_init(&deadline_sem, 0, 1);
   sem_init(&runtime_sem, 0, 1);

   sem_init(&joblog_sem, 0, 1);

   //must call enable() before registering statistics
   state = 0;
}

///This function saves to custom csv file
void Statistics::toFile(const string& filePrefix)
{
   cout << "saving..." << endl;
   ofstream file;

   /************ SAVING MISSED DEADLINES *********/
   // MissedDeadline aux_m;
   // file.open((filePrefix+"_missed_deadlines.csv").data());
   // for(unsigned int c=0;c<missedDeadlines.size();c++) {
   //   aux_m = missedDeadlines[c];
   //   file << aux_m.toString() << endl;
   // }
   // file.close();

   /************ SAVING RUNTIMES *********/
   Runtime aux_r;

   //First sort the vector, then save to file
   sort(runtimes.begin(), runtimes.end(), compareId());
   file.open((filePrefix + "_runtimes.csv").data());
   for (unsigned int c = 0; c < runtimes.size(); c++)
   {
      aux_r = runtimes[c];
      file << aux_r.toString() << endl;
   }
   file.close();

   /************ SAVING TRACES *********/
   // Trace aux_t;
   // file.open((filePrefix+"_traces.csv").data());
   // for(unsigned int c=0;c<traces.size();c++) {
   //   aux_t = traces[c];
   //   file << aux_t.toString() << "\n";
   // }
   // file.close();

   //Change the owner and permissions of generated files
   //system(("chown hsf:hsf " + filePrefix + "_*.csv").data() );
   if (system(("chmod 666 " + filePrefix + "_*.csv").data()))
   {
      cout << "error saving file " << endl;
   }

    #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
   if (Scratch::isUsingHardwareTempSensor())
   {
      string tempSaveName = filePrefix + "_hardware_temperature.csv";

      cout << tempSaveName << endl;

      if (Scratch::hardwareOption.isSaveSensorMeanTemperature)
      {
         appendToFile(tempSaveName, localMeanElement(hardwareTemperatures, true));
      }

      if (Scratch::hardwareOption.isSaveGlobalPeakTemperature)
      {
         double maxTemp = maxElement(hardwareTemperatures);
         appendToFile(tempSaveName, vector <double>(1, maxTemp));
      }

      if (Scratch::hardwareOption.isSaveSensorPeakTemperature)
      {
         appendToFile(tempSaveName, localMaxElement(hardwareTemperatures, true));
      }

      if (Scratch::hardwareOption.isSaveSensorTemperatureTrace)
      {
         appendToFile(tempSaveName, hardwareTemperatures);
      }

      if (Scratch::hardwareOption.isSaveSensorReadOverhead)
      {
         tempSaveName = filePrefix + "_hardware_sensor_overhead.csv";
         cout << tempSaveName << endl;
         appendToFile(tempSaveName, hardwareReadingOverheads, 1);
      }
   }
    #endif

    #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (Scratch::isUsingSoftTempSensor())
   {
      string softTempSaveName = filePrefix + "_software_temperature.csv";
      cout << softTempSaveName << endl;


      if (Scratch::softwareOption.isSaveSensorMeanTemperature)
      {
         appendToFile(softTempSaveName, localMeanElement(softwareTemperatures, true));
      }

      if (Scratch::softwareOption.isSaveGlobalPeakTemperature)
      {
         double maxTemp = maxElement(softwareTemperatures);
         appendToFile(softTempSaveName, vector <double>(1, maxTemp));
      }

      if (Scratch::softwareOption.isSaveSensorPeakTemperature)
      {
         appendToFile(softTempSaveName, localMaxElement(softwareTemperatures, true));
      }

      if (Scratch::softwareOption.isSaveSensorTemperatureTrace)
      {
         appendToFile(softTempSaveName, softwareTemperatures);
      }

      if (Scratch::softwareOption.isSavePerformanceCounterValues)
      {
         string papiValueName = filePrefix + "_performance_counter_value.csv";
         cout << papiValueName << endl;
         appendToFile(papiValueName, performanceCounterValues);
      }

      if (Scratch::softwareOption.isSaveSensorReadOverhead)
      {
         string softOverheadName = filePrefix + "_software_sensor_overhead.csv";
         cout << softOverheadName << endl;
         appendToFile(softOverheadName, softwareReadingOverheads, 1);

         string papiOverheadName = filePrefix + "_performance_counter_reading_overhead.csv";
         cout << papiOverheadName << endl;
         appendToFile(papiOverheadName, performanceCounterReadingOverheads);
      }
   }
    #endif
}
