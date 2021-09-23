#ifndef _TIMEUTIL_H
#define _TIMEUTIL_H

#include "utils/Enumerations.h"

#include <stdlib.h>
#include <time.h>
#include <vector>

/***************************************
*        CLASS DECLARATION            *
***************************************/

class TimeUtil {
public:

   /*********** VARIABLES ***********/

   ///This variable holds the (absolute) time at which the simulation started (used to calculate relative time)
   static struct timespec offset;

   /*********** MEMBER FUNCTIONS ***********/

   ///This function returns a timespec with the current time
   static struct timespec getTime();

   ///This function returns a timespec with the current time
   static struct timespec getTime(enum _time_type type);

   ///This function returns an unsigned long int with the current time in microseconds
   static unsigned long getTimeUSec();

   ///This function returns an unsigned long int with the current time in milliseconds
   static unsigned long getTimeMSec();

   ///This function converts a timestamp from absolute time to relative simulation time
   static struct timespec relative(struct timespec absolute);

   //Sets the relative-time offset to current time
   static void setOffset();

   /*********** AUXILIARY FUNCTIONS ***********/

   static std::vector <struct timespec> relative2absloute(
      struct timespec offset, const std::vector <unsigned long>& relativeTimes);

   static struct timespec relative2absloute(
      struct timespec offset, unsigned long relativeTime);

   ///Converts a timespec to unsigned long usecs
   static unsigned long convert_us(struct timespec t1);

   ///Converts a timespec to unsigned long usecs
   static std::vector <unsigned long> convert_us(std::vector <struct timespec> t1);

   ///Converts a timespec to unsigned long msecs
   static unsigned long convert_ms(struct timespec t1);

   ///This function returns a timespec with the specified microseconds
   static struct timespec Micros(long int us);

   ///This function returns a timespec with the specified microseconds
   static std::vector <struct timespec> Micros(std::vector <long int> us);

   ///This function returns a timespec with the specified milliseconds
   static struct timespec Millis(long int ms);

   ///This function returns a timespec with the specified seconds
   static struct timespec Seconds(__time_t s);

   static unsigned long ms2us(double ms);


   static double us2ms(unsigned long us);
};

#endif
