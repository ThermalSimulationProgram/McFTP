#include "TempWatcher.h"

#include <iostream>


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
		cerr << "TempWatcher::TempWatcher: Error, period is zero!\n";
		exit(-1);
	}
  sem_init(&temp_sem, 0, 1);

  sem_wait(&temp_sem);
  curTemp = get_cpu_temperature();
  sem_post(&temp_sem);
  
  samplingPeriod = TimeUtil::Micros(period);


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
  cout << "TempWatcher: " << id << " waiting for initialization\n";
  Semaphores::print_sem.post_sem();
  #endif


  //Wait until the simulation is initialized
  sem_wait(&CMI::init_sem);
  // while( !CMI::isInitialized() ){}

  #if _INFO == 1
  Semaphores::print_sem.wait_sem();
  cout << "TempWatcher: " << id << " begining execution \n";
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
  cout << "TempWatcher " << id << " exiting wrapper...\n";
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
    FILE *f;
    const char* n[] = {	"/sys/class/hwmon/hwmon1/temp2_input",
    "/sys/class/hwmon/hwmon1/temp3_input",
    "/sys/class/hwmon/hwmon1/temp4_input",
    "/sys/class/hwmon/hwmon1/temp5_input"};

    for ( int i = 0; i < TEMP_IDX_MAX; ++i) {
      if ( ( f = fopen( n[i], "r"))) {
        int res, err = 0;

        errno = 0;
        res = fscanf( f, "%d", &value);
        if ( res == EOF && errno == EIO)
          err = -SENSORS_ERR_IO;
        else if ( res != 1)
          err = -SENSORS_ERR_ACCESS_R;
        res = fclose( f);
        if ( err){
         std::cerr << " TempWatcher::get_cpu_temperature: read temperature error, NO:" << err <<std::endl;
         exit(err);
       }

       if ( res == EOF) {
        if ( errno == EIO){
         std::cerr << " TempWatcher::get_cpu_temperature: read temperature error, NO:" << -SENSORS_ERR_IO <<std::endl;
         exit(-SENSORS_ERR_IO) ;
       }
       else{
         std::cerr << " TempWatcher::get_cpu_temperature: read temperature error, NO:" << -SENSORS_ERR_ACCESS_R <<std::endl;
         exit(-SENSORS_ERR_ACCESS_R) ;
       }
     }
            // value /= get_type_scaling( SENSORS_SUBFEATURE_TEMP_INPUT);
   } else{
     std::cout << " TempWatcher::get_cpu_temperature: read temperature error, NO:" << -SENSORS_ERR_KERNEL <<std::endl;
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
