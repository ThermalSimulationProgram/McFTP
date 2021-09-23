#include "TempWatcher.h"

// #include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>


#include "core/Processor.h"
#include "configuration/Scratch.h"
#include "utils/vectormath.h"
#include "pthread/Priorities.h"
#include "utils/Semaphores.h"
#include "utils/FileOperator.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
#include "utils/utils.h"
#include "utils/r8lib.hpp"
#include "utils/matrix_exponential.hpp"
#include "core/ComponentsDefine.h"
#include "soft_temperature_sensor/LinearTemperatureSensor.h"
#include "results/Statistics.h"



using namespace std;


#define _INFO     0
#define _DEBUG    0



/***************************************
*        CLASS DEFINITION             *
***************************************/

TempWatcher::TempWatcher(unsigned period, unsigned _id, Processor *p)
   : TempWatcher(period, _id, true, false, p)
{
}

TempWatcher::TempWatcher(unsigned period, unsigned _id, bool useHardware,
                         bool useSoft, Processor *p) : Thread(_id)
{
   thread_type = _temp_watcher;
   if (period == 0)
   {
      printf("TempWatcher::TempWatcher: Error, period is zero!\n");
      exit(-1);
   }
   sem_init(&temp_sem, 0, 1);

   samplingPeriod = TimeUtil::Micros(period);

   useHardwareSensor = useHardware;

   // if (useHardwareSensor){
   //    hardwareTempTrace.reserve(Scratch::getDuration()/period);
   // }

   useSoftSensor = useSoft;

    #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (useSoftSensor)
   {
      // softTempTrace.clear();
      // softTempTrace.reserve(Scratch::getDuration()/period);
      // performanceCounterValues.reserve(Scratch::getDuration()/period);
      // softSensorOverhead.reserve(Scratch::getDuration()/period);
      PerformanceCounters::initializeLibrary();
   }
    #endif

   processor = p;
}

TempWatcher::~TempWatcher()
{
}

void TempWatcher::activate()
{
   setPriority(Priorities::get_tempwatcher_pr());
}

void TempWatcher::join()
{
   // cout << "TempWatcher::joining" << endl;
   join2();
}

void TempWatcher::wrapper()
{
    #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (useSoftSensor)
   {
      Scratch::softSensor = LinearTemperatureSensor;
      initializeLinearTemperatureSensor();
   }
    #endif


    #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   printf("TempWatcher: waiting for initialization\n");
   Semaphores::print_sem.post_sem();
    #endif


   //Wait until the simulation is initialized
   sem_wait(&Processor::init_sem);

    #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   printf("TempWatcher: begining execution \n");
   Semaphores::print_sem.post_sem();
    #endif

   ///wait for the simulation start
   sem_wait(&Processor::running_sem);

   sem_t periodic_sem;
   sem_init(&periodic_sem, 0, 0);
   struct timespec waitEnd;

   while (Processor::isRunning())
   {
      waitEnd = TimeUtil::getTime() + samplingPeriod;

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
      if (useSoftSensor)
      {
         processor->triggerAllPAPIReading(TEMPWATCHER);
      }
        #endif

      sem_wait(&temp_sem);
        #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
      if (useHardwareSensor)
      {
         curHardwareTemp = get_cpu_temperature();
         Statistics::addHardwareTemperature(curHardwareTemp);
      }
        #endif

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
      if (useSoftSensor)
      {
         curSoftTemp = get_soft_cpu_temperature();
         if (curSoftTemp.size() == 0)
         {
            cout << "temperature size error" << endl;
            exit(1);
         }
         Statistics::addSoftwareTemperature(curSoftTemp);
      }
        #endif
      sem_post(&temp_sem);

      sem_timedwait(&periodic_sem, &waitEnd);
   }

   // #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   printf("TempWatcher: %d is exiting wrapper...\n", id);
   Semaphores::print_sem.post_sem();
   // #endif
}

bool TempWatcher::isSoftSensorEnabled()
{
   return(useSoftSensor);
}

vector <double> TempWatcher::getCurHardwareTemp()
{
   vector <double> ret;

   sem_wait(&temp_sem);
   ret = curHardwareTemp;
   sem_post(&temp_sem);

   return(ret);
}

vector <double> TempWatcher::getCurSoftwareTemp()
{
   vector <double> ret;

   sem_wait(&temp_sem);
   ret = curSoftTemp;
   sem_post(&temp_sem);

   return(ret);
}

vector <double> TempWatcher::get_cpu_temperature()
{
   vector <double> ret;

    #ifdef HARD_TEMPERATURE_SENSOR_ENABLE
   if (useHardwareSensor)
   {
      struct timespec start = TimeUtil::getTime();

      int value;

      for (int i = 0; i < (int)Scratch::hardwareSensorPath.size(); ++i)
      {
         int fd = open(Scratch::hardwareSensorPath[i].c_str(), O_RDONLY);
         if (fd != -1)
         {
            char    buf[12];
            ssize_t numwrite = read(fd, buf, 12);

            if (numwrite < 1)
            {
               close(fd);
               printf(" TempWatcher::get_cpu_temperature: read temperature error, can not read the file\n");
               exit(1);
            }

            close(fd);

            sscanf(buf, "%d", &value);
         }
         else
         {
            printf(" TempWatcher::get_cpu_temperature: read temperature error, NO:");
            exit(-1);
         }

         ret.push_back((double)value);
      }

      Statistics::addHardwareReadingOverhead(
         TimeUtil::convert_us(TimeUtil::getTime() - start));
   }
    #endif

   return(ret);
}

vector <double> TempWatcher::get_soft_cpu_temperature()
{
   if (useSoftSensor)
   {
      // all workers have finished reading PAPI values
      if (processor->waitPAPIReading() == 0)
      {
         struct timespec start = TimeUtil::getTime();

         vector <vector <long long> > allPAPIValues;
         processor->getPAPIValues(allPAPIValues);

         vector <long long> values;

         for (int i = 0; i < (int)allPAPIValues.size(); ++i)
         {
            for (int j = 0; j < (int)allPAPIValues[i].size(); ++j)
            {
               values.push_back(allPAPIValues[i][j]);
            }
         }

         vector <double> temperature = Scratch::softSensor(values);

         Statistics::addSoftwareReadingOverhead(
            TimeUtil::convert_us(TimeUtil::getTime() - start));
         Statistics::addPerformanceCounterValue(values);

         //
         return(temperature);
      }
      else
      {
         return(vector <double>{ 0, 0, 0, 0 });
      }
   }
   else
   {
      return(vector <double>{ 0, 0, 0, 0 });
   }
}

// double TempWatcher::calcMaxTemp(vector<vector<double> > temp){
//     vector<double> s_max;
//     s_max.reserve(temp.size());
//     for (unsigned i = 0; i < temp.size(); ++i)
//     {
//       s_max.push_back(maxElement(temp[i]));
//     }

//     return maxElement(s_max);
// }

// double TempWatcher::calcMeanMaxTemp(vector<vector<double> > temp){
//     vector<double> s_max;
//     s_max.reserve(temp.size());
//     for (unsigned i = 0; i < temp.size(); ++i)
//     {
//         s_max.push_back(maxElement(temp[i]));
//     }
//     double sum = accumulate(s_max.begin(), s_max.end(), 0.0);
//     double avg = sum/s_max.size();

//     return avg;
// }

// vector<double> TempWatcher::calcMeanTemp(vector<vector<double> > temp){
//     vector<double> ret;
//     if (temp.size() == 0){
//         return ret;
//     }
//     unsigned nstage = temp[0].size();

//     for (unsigned i = 0; i < nstage; ++i)
//     {
//         vector<double> singleStageTempTrace;
//         singleStageTempTrace.reserve(temp.size());

//         for (unsigned j = 0; j < temp.size(); ++j)
//         {
//           singleStageTempTrace.push_back(temp[j][i]);
//         }

//       double sum = std::accumulate(singleStageTempTrace.begin(),
//           singleStageTempTrace.end(), 0.0);
//       if (singleStageTempTrace.size() == 0){
//         printf( " TempWatcher::calcMeanTemp: temperature trace is empty!\n");
//         exit(-1);
//       }
//       double avg = sum/singleStageTempTrace.size();
//       ret.push_back(avg);
//     }
//     return ret;
// }



// double TempWatcher::getMaxTemp(){
//     return calcMaxTemp(hardwareTempTrace);
// }

// double TempWatcher::getMeanMaxTemp(){

//     return calcMeanMaxTemp(hardwareTempTrace);
// }


// vector<double> TempWatcher::getMeanTemp(){
//     return calcMeanTemp(hardwareTempTrace);
// }

// vector<vector<double> > TempWatcher::getAllTempTrace(){
//     return hardwareTempTrace;
// }


// double TempWatcher::getMaxSoftSensorTemp(){
//     return calcMaxTemp(softTempTrace);
// }

// double TempWatcher::getMeanMaxSoftSensorTemp(){
//     return calcMeanMaxTemp(softTempTrace);
// }

// vector<double> TempWatcher::getMeanSoftSensorTemp(){
//     return calcMeanTemp(softTempTrace);
// }

// vector<vector<double> > TempWatcher::getAllSoftSensorTempTrace(){
//     return softTempTrace;
// }

// vector<long> TempWatcher::getSoftSensorOverhead(){
//     return softSensorOverhead;
// }
