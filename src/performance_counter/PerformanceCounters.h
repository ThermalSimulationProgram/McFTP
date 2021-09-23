#ifndef MCFTP_PERFORMANCECOUNTERS_H
#define MCFTP_PERFORMANCECOUNTERS_H

#define MAX_EVENT_COUNTER_NUMBER    100

#include <vector>
#include <string>

#include "../McFTPBuildConfig.h"

// #include <pthread.h>

class PerformanceCounters {
protected:
   bool initialized;

   int PAPI_Counter_Set;

   long long values[MAX_EVENT_COUNTER_NUMBER];

   int counterNumber;

   static bool library_initialized;

   // static pthread_key_t key;


public:

   PerformanceCounters();

   bool initializeCounterSet();

   static bool initializeLibrary();

   static bool initializeThread();

   bool addCounter(int counterId);

   bool addCounter(const std::string counterName);

   bool startAllCounters();

   bool stopAllCounters();

   bool readAllValues();

   bool resetAllCounters();

   // long long getCounterValue(const std::string counterName);

   long long getCounterValue(int counterId);

   int getCounterNumber();

   bool exitThread();

   // void endAllCounters();

   void clearCounterValues();

   // static void printCounterInfo();

   // static void PAPI_test(int argc, char **argv);
};

#endif
