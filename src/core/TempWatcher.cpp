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


using namespace std;


#define _INFO 0
#define _DEBUG 0




/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

TempWatcher::TempWatcher(unsigned period, unsigned _id):TempWatcher(period, _id, true, false){

	
}	

TempWatcher::TempWatcher(unsigned period, unsigned _id, bool useHardware, bool useSoft):Thread(_id){
    thread_type = _temp_watcher;
    if (period == 0){
        printf("TempWatcher::TempWatcher: Error, period is zero!\n");
        exit(-1);
    }
    sem_init(&temp_sem, 0, 1);

    sem_wait(&temp_sem);
    curHardwareTemp = get_cpu_temperature();
    sem_post(&temp_sem);

    samplingPeriod = TimeUtil::Micros(period);

    hardwareTempTrace.reserve(Scratch::getDuration()/200000); 

    useHardwareSensor = useHardware;

    useSoftSensor = useSoft;  

    temperatureCounters = PerformanceCounters();

    if (useSoftSensor){
        temperatureCounters.initialize();
        softSensor = Scratch::softSensor;
        for (int i = 0; i < (int) Scratch::soft_sensors.size(); ++i)
        {
            addSoftLinearTemperatureSensor(Scratch::soft_sensors[i].counterName,
                Scratch::soft_sensors[i].coefA, Scratch::soft_sensors[i].coefB); 
        }
    }
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
    sem_wait(&Processor::init_sem);

    #if _INFO == 1
    Semaphores::print_sem.wait_sem();
    printf("TempWatcher: begining execution \n");
    Semaphores::print_sem.post_sem();
    #endif

	///wait for the simulation start
    sem_wait(&Processor::running_sem);

    if (useSoftSensor){
        if (! temperatureCounters.startAllCounters() ){
            printf("TempWatcher::wrapper: failed to start performance counters\n");
            exit(1);
        }
    }


    while(Processor::isRunning()){
        sem_wait(&temp_sem);
        curHardwareTemp = get_cpu_temperature();
        curSoftTemp = get_soft_cpu_temperature();
        sem_post(&temp_sem);
        hardwareTempTrace.push_back(curHardwareTemp);
        softTempTrace.push_back(curSoftTemp);

        nanosleep(&samplingPeriod, &rem);
    }

	#if _INFO == 1
    Semaphores::print_sem.wait_sem();
    printf( "TempWatcher  exiting wrapper...\n");
    Semaphores::print_sem.post_sem();
    #endif
}

bool TempWatcher::isSoftSensorEnabled(){
    return useSoftSensor;
}

bool TempWatcher::addSoftLinearTemperatureSensor(std::string counterName, double a, double b){
    if (useSoftSensor){
        temperatureCounters.initialize();

        if (! temperatureCounters.addCounter(counterName) ) {
            printf("Warning: TempWatcher::addSoftLinearTemperatureSensor failed to add performance counter \n");
            return false;
        }else{
            coefAVector.push_back(a);
            coefBVector.push_back(b);
            return true;
        }
    }else{
        printf("Warning: TempWatcher::addSoftLinearTemperatureSensor failed to add soft sensor because soft temperature sensor is not enabled!\n");
        return false;
    }
}
vector<double> TempWatcher::getCurTemp(){
    vector<double> ret;
    sem_wait(&temp_sem);
    ret = curHardwareTemp ;
    sem_post(&temp_sem);

    return ret;
}


std::vector<double> TempWatcher::get_cpu_temperature(){
	#if _DEBUG == 1
    std::vector<double> ret(4, 30);
    #endif

    #if _DEBUG == 0
    std::vector<double> ret;
    if (useHardwareSensor){
        
        int value;
        int TEMP_IDX_MAX = 4;

    const char* n[] = {  "/sys/class/hwmon/hwmon1/temp2_input",
    "/sys/class/hwmon/hwmon1/temp3_input",
    "/sys/class/hwmon/hwmon1/temp4_input",
    "/sys/class/hwmon/hwmon1/temp5_input"};

        // const char* n[] = { "/home/long/test/test1",
        // "/home/long/test/test2",
        // "/home/long/test/test3",
        // "/home/long/test/test4"};



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
    }

    


    #endif

    return ret;
}

std::vector<double> TempWatcher::get_soft_cpu_temperature(){
    std::vector<double> ret;
    if (useSoftSensor){
        if (!temperatureCounters.readAllValues()){
            cout << "warning: TempWatcher::get_soft_cpu_temperature: failed to read performance counter values\n";

        }
        

        if (softSensor == NULL){
            for (int i = 0; i < temperatureCounters.getCounterNumber(); ++i)
            {
                long long v = temperatureCounters.getCounterValue(i);
                cout << "debug: performanc counter value: " << v << endl;
                double temp = coefAVector[i] * v + coefBVector[i];
                ret.push_back(temp);
            } 
        }else{
            vector<long long> values;
            for (int i = 0; i < temperatureCounters.getCounterNumber(); ++i)
            {
                values.push_back(temperatureCounters.getCounterValue(i));  
            } 

            ret = softSensor(values);
        }
    }

    return ret;
}

double TempWatcher::calcMaxTemp(std::vector<std::vector<double> > temp){
    vector<double> s_max;
    s_max.reserve(temp.size());
    for (unsigned i = 0; i < temp.size(); ++i)
    {
      s_max.push_back(maxElement(temp[i]));
  }
  return maxElement(s_max);
}
double TempWatcher::calcMeanMaxTemp(std::vector<std::vector<double> > temp){
    vector<double> s_max;
    s_max.reserve(temp.size());
    for (unsigned i = 0; i < temp.size(); ++i)
    {
        s_max.push_back(maxElement(temp[i]));
    }
    double sum = std::accumulate(s_max.begin(), s_max.end(), 0.0);
    double avg = sum/s_max.size();

    return avg;

}
std::vector<double> TempWatcher::calcMeanTemp(std::vector<std::vector<double> > temp){
    vector<double> ret;
    unsigned nstage = temp.size();

    for (unsigned i = 0; i < nstage; ++i)
    {
        vector<double> singleStageTempTrace;
        singleStageTempTrace.reserve(temp.size());

        for (unsigned j = 0; j < temp.size(); ++j)
        {
          singleStageTempTrace.push_back(temp[j][i]);
      }

      double sum = std::accumulate(singleStageTempTrace.begin(), 
          singleStageTempTrace.end(), 0.0);
      double avg = sum/singleStageTempTrace.size();
      ret.push_back(avg);
    }

    return ret;
}



double TempWatcher::getMaxTemp(){
    return calcMaxTemp(hardwareTempTrace);
}

double TempWatcher::getMeanMaxTemp(){

    return calcMeanMaxTemp(hardwareTempTrace);
}


vector<double> TempWatcher::getMeanTemp(){

    return calcMeanTemp(hardwareTempTrace);

}

std::vector<std::vector<double> > TempWatcher::getAllTempTrace(){
    return hardwareTempTrace;
}


double TempWatcher::getMaxSoftSensorTemp(){
    return calcMaxTemp(softTempTrace);
}

double TempWatcher::getMeanMaxSoftSensorTemp(){
    return calcMeanMaxTemp(softTempTrace);
}

std::vector<double> TempWatcher::getMeanSoftSensorTemp(){
    return calcMeanTemp(softTempTrace);
}

std::vector<std::vector<double> > TempWatcher::getAllSoftSensorTempTrace(){
    return softTempTrace;
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
