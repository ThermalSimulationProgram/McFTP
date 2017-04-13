#include "TempWatcher.h"

// #include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>


#include "core/CMI.h"
#include "configuration/Scratch.h"
#include "utils/vectormath.h"
#include "pthread/Priorities.h"
#include "utils/Semaphores.h"
#include "utils/FileOperator.h"
#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"

using namespace std;


#define _INFO 0
#define _DEBUG 1




/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

TempWatcher::TempWatcher(unsigned period, unsigned _id):Thread(_id){

	thread_type = _temp_watcher;
	if (period == 0){
		printf("TempWatcher::TempWatcher: Error, period is zero!\n");
		exit(-1);
	}
  sem_init(&temp_sem, 0, 1);

  sem_wait(&temp_sem);
  curTemp = get_cpu_temperature();
  sem_post(&temp_sem);
  
  samplingPeriod = TimeUtil::Micros(period);

  tempTrace.reserve(Scratch::getDuration()/200000);


}	


TempWatcher::~TempWatcher(){
  // cout << "temperature watcher with id " << id << " is being destructed\n";
}

void TempWatcher::activate(){
	setPriority(Priorities::get_tempwatcher_pr());
}

void TempWatcher::join(){
  // cout << "TempWatcher::joining" << endl;
	join2();
}


void TempWatcher::wrapper(){
  struct timespec rem;
  #if _INFO == 1
  Semaphores::print_sem.wait_sem();
  printf("TempWatcher: waiting for initialization\n");
  Semaphores::print_sem.post_sem();
  #endif


  //Wait until the simulation is initialized
  sem_wait(&CMI::init_sem);
  // while( !CMI::isInitialized() ){}

  #if _INFO == 1
  Semaphores::print_sem.wait_sem();
  printf("TempWatcher: begining execution \n");
  Semaphores::print_sem.post_sem();
  #endif

	///wait for the simulation start
  sem_wait(&CMI::running_sem);
  // while(!CMI::isRunning()){}

  while(CMI::isRunning()){
    sem_wait(&temp_sem);
    curTemp = get_cpu_temperature();
    sem_post(&temp_sem);
    tempTrace.push_back(curTemp);

    nanosleep(&samplingPeriod, &rem);
  }

	#if _INFO == 1
  Semaphores::print_sem.wait_sem();
  printf( "TempWatcher  exiting wrapper...\n");
  Semaphores::print_sem.post_sem();
  #endif
}



vector<double> TempWatcher::getCurTemp(){
  vector<double> ret;
  sem_wait(&temp_sem);
  ret = curTemp ;
  sem_post(&temp_sem);

  return ret;
}


std::vector<double> TempWatcher::get_cpu_temperature(){
	#if _DEBUG == 1
    std::vector<double> ret(4, 30);
  #endif

    #if _DEBUG == 0
    std::vector<double> ret;
    int value;
    int TEMP_IDX_MAX = 4;
    
    const char* n[] = {	"/sys/class/hwmon/hwmon1/temp2_input",
    "/sys/class/hwmon/hwmon1/temp3_input",
    "/sys/class/hwmon/hwmon1/temp4_input",
    "/sys/class/hwmon/hwmon1/temp5_input"};

    int fd;

    for ( int i = 0; i < TEMP_IDX_MAX; ++i) {
      fd = open(n[i], O_RDONLY);
      if (fd != -1 ) {

        char buf[12];
        ssize_t numwrite = read(fd, buf, 12);

        if (numwrite < 1) {
          close(fd);
          printf(" TempWatcher::get_cpu_temperature: read temperature error, can not read the file\n"); 
          exit(1);
        }

        close(fd);

        sscanf(buf, "%d", &value);
   } else{
     printf( " TempWatcher::get_cpu_temperature: read temperature error, NO:");
     printf("%d\n", SENSORS_ERR_KERNEL); 
     exit(-SENSORS_ERR_KERNEL) ;
   }

   ret.push_back((double)value);
 }
#endif

return ret;
}

double TempWatcher::getMaxTemp(){
	vector<double> s_max;
  s_max.reserve(tempTrace.size());
	for (unsigned i = 0; i < tempTrace.size(); ++i)
	{
		s_max.push_back(maxElement(tempTrace[i]));
	}
 return maxElement(s_max);
  // double sum = std::accumulate(s_max.begin(), s_max.end(), 0.0);
  // double avg = sum/s_max.size();
	
  // return avg;

}

double TempWatcher::getMeanMaxTemp(){
  vector<double> s_max;
  s_max.reserve(tempTrace.size());
  for (unsigned i = 0; i < tempTrace.size(); ++i)
  {
    s_max.push_back(maxElement(tempTrace[i]));
  }
  double sum = std::accumulate(s_max.begin(), s_max.end(), 0.0);
  double avg = sum/s_max.size();
  
  return avg;
}


vector<double> TempWatcher::getMeanTemp(){
  vector<double> ret;
  unsigned nstage = curTemp.size();

  for (unsigned i = 0; i < nstage; ++i)
  {
    vector<double> singleStageTempTrace;
    singleStageTempTrace.reserve(tempTrace.size());

    for (unsigned j = 0; j < tempTrace.size(); ++j)
    {
      singleStageTempTrace.push_back(tempTrace[j][i]);
    }

    double sum = std::accumulate(singleStageTempTrace.begin(), 
      singleStageTempTrace.end(), 0.0);
    double avg = sum/singleStageTempTrace.size();
    ret.push_back(avg);
  }

  return ret;

}



// int get_type_scaling(sensors_subfeature_type type)
// {
// 	/* Multipliers for subfeatures */
// 	switch (type & 0xFF80) {
// 	case SENSORS_SUBFEATURE_IN_INPUT:
// 	case SENSORS_SUBFEATURE_TEMP_INPUT:
// 	case SENSORS_SUBFEATURE_CURR_INPUT:
// 	case SENSORS_SUBFEATURE_HUMIDITY_INPUT:
// 		return 1000;
// 	case SENSORS_SUBFEATURE_FAN_INPUT:
// 		return 1;
// 	case SENSORS_SUBFEATURE_POWER_AVERAGE:
// 	case SENSORS_SUBFEATURE_ENERGY_INPUT:
// 		return 1000000;
// 	}

// 	/* Multipliers for second class subfeatures
// 	   that need their own multiplier */
// 	switch (type) {
// 	case SENSORS_SUBFEATURE_POWER_AVERAGE_INTERVAL:
// 	case SENSORS_SUBFEATURE_VID:
// 	case SENSORS_SUBFEATURE_TEMP_OFFSET:
// 		return 1000;
// 	default:
// 		return 1;
// 	}
// }
std::vector<std::vector<double> > TempWatcher::getAllTempTrace(){
  return tempTrace;
}