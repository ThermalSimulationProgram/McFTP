#include "utils/TimeUtil.h"
#include "utils/Enumerations.h"
#include "utils/Clock.h"
#include "utils/Operators.h"

#include <time.h>

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** STATIC VARIABLES ***********/

struct timespec TimeUtil::offset;

/*********** MEMBER FUNCTIONS ***********/

///This function returns a timespec with the current time
struct timespec TimeUtil::getTime()
{
   struct timespec aux;

   clock_gettime(HSF_CLOCK, &aux);
   return(aux);
}

///This function returns a timespec with the current time
struct timespec TimeUtil::getTime(enum _time_type _type)
{
   struct timespec aux;

   clock_gettime(HSF_CLOCK, &aux);

   if (_type == _relative)
   {
      aux = aux - offset;
   }

   return(aux);
}

///This function returns an unsigned long int with the current time in microseconds
unsigned long TimeUtil::getTimeUSec()
{
   return(convert_us(getTime(_relative)));
}

unsigned long TimeUtil::getTimeMSec()
{
   return(convert_ms(getTime(_relative)));
}

///This function converts a timestamp from absolute time to relative simulation time
struct timespec TimeUtil::relative(struct timespec absolute)
{
   return(absolute - offset);
}

///Sets the relative-time offset to current time
void TimeUtil::setOffset()
{
   clock_gettime(HSF_CLOCK, &offset);
}

/*********** AUXILIARY FUNCTIONS ***********/

std::vector <struct timespec> TimeUtil::relative2absloute(
   struct timespec offset, const std::vector <unsigned long>& relativeTimes)
{
   std::vector <struct timespec> abs_times;
   unsigned long offset_micro;

   offset_micro = convert_us(offset);
   abs_times.reserve(relativeTimes.size());

   unsigned long aux;

   for (unsigned i = 0; i < relativeTimes.size(); ++i)
   {
      aux = offset_micro + relativeTimes[i];
      abs_times.push_back(TimeUtil::Micros(aux));
   }

   return(abs_times);
}

struct timespec TimeUtil::relative2absloute(
   struct timespec offset, unsigned long relativeTime)
{
   struct timespec absoluteTime;
   unsigned long   t;

   t            = convert_us(offset) + relativeTime;
   absoluteTime = Micros(t);

   return(absoluteTime);
}

std::vector <unsigned long> TimeUtil::convert_us(std::vector <struct timespec> t1)
{
   std::vector <unsigned long> ret;

   for (int i = 0; i < (int)t1.size(); ++i)
   {
      ret.push_back(convert_us(t1[i]));
   }
   return(ret);
}

///Converts a timespec to unsigned long usecs
unsigned long TimeUtil::convert_us(struct timespec t1)
{
   unsigned long ul = (unsigned long int)(t1.tv_sec) * 1000000 + (unsigned long int)(t1.tv_nsec) / 1000;

   return(ul);
}

///Converts a timespec to unsigned long msecs
unsigned long TimeUtil::convert_ms(struct timespec t1)
{
   unsigned long ul = (unsigned long int)(t1.tv_sec) * 1000 + (unsigned long int)(t1.tv_nsec) / 1000000;

   return(ul);
}

///This function returns a timespec with the specified microseconds
std::vector <struct timespec> TimeUtil::Micros(std::vector <long int> us)
{
   std::vector <struct timespec> ret;

   for (int i = 0; i < (int)us.size(); ++i)
   {
      ret.push_back(Micros(us[i]));
   }
   return(ret);
}

///This function returns a timespec with the specified microseconds
struct timespec TimeUtil::Micros(long int us)
{
   struct timespec aux;

   if (us < 1000000)
   {
      aux.tv_sec  = 0;
      aux.tv_nsec = us * 1000;
   }
   else
   {
      aux.tv_sec  = (int)(us / 1000000);
      aux.tv_nsec = (us - aux.tv_sec * 1000000) * 1000;
   }

   return(aux);
}

///This function returns a timespec with the specified milliseconds
struct timespec TimeUtil::Millis(long int ms)
{
   struct timespec aux;


   if (ms < 1000)
   {
      aux.tv_sec  = 0;
      aux.tv_nsec = ms * 1000000;
   }
   else
   {
      aux.tv_sec  = (int)(ms / 1000);
      aux.tv_nsec = (ms - aux.tv_sec * 1000) * 1000000;
   }

   return(aux);
}

///This function returns a timespec with the specified seconds
struct timespec TimeUtil::Seconds(__time_t s)
{
   struct timespec aux;

   aux.tv_sec  = s;
   aux.tv_nsec = 0;

   return(aux);
}

unsigned long TimeUtil::ms2us(double ms)
{
   return((unsigned long)(ms * 1000 + 0.5));
}

double TimeUtil::us2ms(unsigned long us)
{
   return((double)us / 1000);
}
