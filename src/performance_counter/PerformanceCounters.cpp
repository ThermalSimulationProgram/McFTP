#include "PerformanceCounters.h"


#include <iostream>
#include <cstring>

#ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
extern "C" {
#include "../../third_party_lib/papi-5.5.1/src/testlib/papi_test.h"
}
#endif


#include "performance_counter/event_info.h"

using namespace std;


bool PerformanceCounters::library_initialized = false;


PerformanceCounters::PerformanceCounters()
{
   initialized   = false;
   counterNumber = 0;
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   PAPI_Counter_Set = PAPI_NULL;
        #endif
}

bool PerformanceCounters::initializeLibrary()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (library_initialized)
   {
      return(true);
   }

   int retval = PAPI_library_init(PAPI_VER_CURRENT);
   if (retval != PAPI_VER_CURRENT)
   {
      cout << "Library PAPI: Performance Application Programming Interface initialize library failed " << endl;
      exit(1);
   }

   retval = PAPI_thread_init((unsigned long (*)(void))(pthread_self));
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: PAPI_thread_init failed" << endl;
      exit(1);
   }

   // retval = PAPI_set_granularity(PAPI_GRN_SYS);
   // if ( retval != PAPI_OK ){
   //    cout << "Library PAPI: PAPI_set_granularity failed" << endl;
   //    exit(1);
   // }

   library_initialized = true;
        #endif
   return(true);
}

bool PerformanceCounters::initializeCounterSet()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   int retval = PAPI_create_eventset(&PAPI_Counter_Set);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface initialize event set failed! " << retval << endl;
      exit(1);
   }

   initialized = true;
        #endif
   return(true);
}

bool PerformanceCounters::initializeThread()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (!library_initialized)
   {
      cout << "Library PAPI is not initialized! Failed to register a thread." << endl;
      return(false);
   }

   int retval = PAPI_register_thread();
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: initialize thread failed! error code " << retval << endl;
      exit(1);
   }

   retval = PAPI_set_domain(PAPI_DOM_ALL);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: PAPI_set_domain failed" << endl;
      exit(1);
   }
        #endif
   return(true);
}

bool PerformanceCounters::addCounter(int counterId)
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (!initialized)
   {
      cout << "Library PAPI thread is not registered! Failed to add new event." << endl;
      return(false);
   }
   int retval = PAPI_add_event(PAPI_Counter_Set, counterId);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to add new event to the set " << endl;
      PAPI_cleanup_eventset(PAPI_Counter_Set);
      counterNumber = 0;
      return(false);
   }
   else
   {
      // cout << "Library PAPI: Successfully add counter with id: " << counterId << endl;
   }
   ++counterNumber;
        #endif
   return(true);
}

bool PerformanceCounters::addCounter(const string counterName)
{
   bool ret = true;

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (!initialized)
   {
      cout << "Library PAPI thread is not registered! Failed to add new event." << endl;
      return(false);
   }
   char *_counterName = new char[counterName.length() + 1];
   strcpy(_counterName, counterName.c_str());
   int _counterId;
   int retval = PAPI_event_name_to_code(_counterName, &_counterId);
   delete[] _counterName;
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to find counter: " << counterName << endl;
      return(false);
   }
   ret = addCounter(_counterId);
        #endif
   return(ret);
}

bool PerformanceCounters::startAllCounters()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (!initialized)
   {
      cout << "Library PAPI thread is not registered! Failed to start all event." << endl;
      return(false);
   }

   int retval = PAPI_start(PAPI_Counter_Set);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to start counters added by user" << endl;
      return(false);
   }
        #endif
   return(true);
}

bool PerformanceCounters::readAllValues()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   if (!initialized)
   {
      cout << "Library PAPI thread is not registered! Failed to read event value." << endl;
      return(false);
   }
   int retval = PAPI_read(PAPI_Counter_Set, values);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to read counters" << endl;
      return(false);
   }
        #endif

   return(resetAllCounters());
}

bool PerformanceCounters::resetAllCounters()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   int retval = PAPI_reset(PAPI_Counter_Set);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to reset counters" << endl;
      return(false);
   }
        #endif
   return(true);
}

bool PerformanceCounters::stopAllCounters()
{
        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   int retval = PAPI_stop(PAPI_Counter_Set, values);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: Performance Application Programming Interface failed to stop all counters" << endl;
      return(false);
   }
        #endif
   return(true);
}

bool PerformanceCounters::exitThread()
{
   bool ret = false;

        #ifdef SOFT_TEMPERATURE_SENSOR_ENABLE
   ret = stopAllCounters();

   int retval = PAPI_cleanup_eventset(PAPI_Counter_Set);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: PAPI_cleanup_eventset failed - " << retval << endl;
      ret = false;
   }

   retval = PAPI_destroy_eventset(&PAPI_Counter_Set);
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: PAPI_destroy_eventset failed - " << retval << endl;
      ret = false;
   }

   retval = PAPI_unregister_thread();
   if (retval != PAPI_OK)
   {
      cout << "Library PAPI: PAPI_unregister_thread failed - " << retval << endl;
      ret = false;
   }
    #endif
   return(ret);
}

long long PerformanceCounters::getCounterValue(int counterId)
{
   if (counterId < 0 || counterId >= counterNumber)
   {
      return(0);
   }
   return(values[counterId]);
}

int PerformanceCounters::getCounterNumber()
{
   return(counterNumber);
}

void PerformanceCounters::clearCounterValues()
{
   for (int i = 0; i < counterNumber; ++i)
   {
      values[i] = 0;
   }
}
