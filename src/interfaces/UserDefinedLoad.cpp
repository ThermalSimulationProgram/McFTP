#include "interfaces/UserDefinedLoad.h"

#include <time.h>
#include <cmath>

#include "utils/Operators.h"
#include "utils/TimeUtil.h"
#include "taskloads/MWC.h"


UserDefinedLoad::UserDefinedLoad(int _loadId) : TaskLoad()
{
   count  = 0;
   loadId = _loadId;

   arg = NULL;
}

UserDefinedLoad::~UserDefinedLoad()
{
}

bool UserDefinedLoad::runLoads(unsigned long _wcet_us)
{
   wcet_us = _wcet_us;
   switch (loadId)
   {
   case 1: {
      bool f = coolTask();
      return(f);
   }
   }
   return(false);
}

bool UserDefinedLoad::coolTask()
{
   initCheckCounter();
   unsigned long   fire_start = TimeUtil::convert_us(TimeUtil::getTime());
   struct timespec sleep      = TimeUtil::Micros(20);
   struct timespec rem;
   MWC             RND = MWC();

   do
   {
      setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
      setCheckBlockBegin();

      unsigned long start = 0, tmp_count = 0;
      start = TimeUtil::convert_us(TimeUtil::getTime());
      while (tmp_count < 200)
      {
         for (int i = 0; i < 3000; ++i)
         {
            sqrt(RND.mwc32());
         }
         nanosleep(&sleep, &rem);
         tmp_count = TimeUtil::convert_us(TimeUtil::getTime()) - start;
      }



      unsigned long end = TimeUtil::convert_us(TimeUtil::getTime());
      count     += end - fire_start;
      fire_start = end;

      setCheckBlockEnd();
   } while (count < wcet_us);

   count = 0;
   return(true);
}
