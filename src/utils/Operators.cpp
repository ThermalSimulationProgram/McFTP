#include "utils/Operators.h"

#include <stdlib.h>

using namespace std;

struct timespec operator *(double a, struct timespec b)
{
   struct timespec x;

   if (a < 0)
   {
      x.tv_sec  = a * b.tv_sec;
      x.tv_nsec = -1 * a * b.tv_nsec;

      if (b.tv_sec == 0)
      {
         x.tv_nsec = -1 * x.tv_nsec;
      }
   }
   else
   {
      x.tv_sec  = a * b.tv_sec;
      x.tv_nsec = a * b.tv_nsec;
   }

   while (x.tv_nsec > 999999999)
   {
      x.tv_sec++;
      x.tv_nsec = x.tv_nsec - 1000000000;
   }

   return(x);
}

struct timespec operator +(const struct timespec &a, const struct timespec &b)
{
   struct timespec x;

   x.tv_sec  = a.tv_sec + b.tv_sec;
   x.tv_nsec = a.tv_nsec + b.tv_nsec;

   if (x.tv_nsec > 999999999)
   {
      x.tv_sec++;
      x.tv_nsec = x.tv_nsec - 1000000000;
   }
   else if (x.tv_nsec < 0)
   {
      x.tv_sec--;
      x.tv_nsec = x.tv_nsec + 1000000000;
   }

   return(x);
}

struct timespec operator -(const struct timespec &a, const struct timespec &b)
{
   struct timespec x;

   x.tv_sec = a.tv_sec - b.tv_sec;

   if (a.tv_nsec < b.tv_nsec && x.tv_sec > 0)
   {
      x.tv_sec  = x.tv_sec - 1;
      x.tv_nsec = a.tv_nsec - b.tv_nsec + 1000000000;
   }
   else
   {
      x.tv_nsec = a.tv_nsec - b.tv_nsec;
   }

   return(x);
}

bool operator <(const struct timespec &a, const struct timespec &b)
{
   if (a.tv_sec < b.tv_sec)
   {
      return(true);
   }
   else
   {
      if (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec)
      {
         return(true);
      }
      else
      {
         return(false);
      }
   }
}

bool operator >(const struct timespec &a, const struct timespec &b)
{
   if (a.tv_sec > b.tv_sec)
   {
      return(true);
   }
   else
   {
      if (a.tv_sec == b.tv_sec && a.tv_nsec > b.tv_nsec)
      {
         return(true);
      }
      else
      {
         return(false);
      }
   }
}

bool operator ==(struct timespec a, struct timespec b)
{
   if (a.tv_sec != b.tv_sec)
   {
      return(false);
   }

   if (a.tv_nsec != b.tv_nsec)
   {
      return(false);
   }

   return(true);
}

bool operator <=(struct timespec a, struct timespec b)
{
   return((a < b) || (a == b));
}

bool operator >=(struct timespec a, struct timespec b)
{
   return((a > b) || (a == b));
}
