#include "taskloads/BusyWait.h"

#include <cmath>
#include <iostream>

#include "utils/Operators.h"
#include "utils/TimeUtil.h"

using namespace std;


BusyWait::BusyWait() : TaskLoad()
{
   count = 0;
}

BusyWait::~BusyWait()
{
}

bool BusyWait::runLoads(unsigned long _wcet_us)
{
   initCheckCounter();
   //unsigned long fire_start = TimeUtil::convert_us(TimeUtil::getTime());
   wcet_us = _wcet_us;

   double ret = 0.0;

   // int jj = 0;

   do
   {
      // if (j==0)  cout << jj << "-0 ";
      setSuspendPoint();     // set a suspend point here. When recieves a suspend signal, pause execution here

      ++TASKLOADCOUNTER;
      if (sem_trywait(&stop_sem) == 0)
      {
         TASKLOADSTOPCOUNTER = TASKLOADSTOPCOUNTER > TASKLOADCOUNTER? TASKLOADSTOPCOUNTER: TASKLOADCOUNTER;
         return(false);
      }
      // if (j==0)  cout <<"-1 ";
      // int kk = 0;
      if (TASKLOADCOUNTER >= TASKLOADSTOPCOUNTER)
      {
         // if (j==2)  cout << j << "2 ";
         struct timespec start     = TimeUtil::getTime();
         unsigned long   tmp_count = 0;

         while (tmp_count < 1000)
         {
            // ++kk;
            for (int i = 0; i < 3000; ++i)
            {
               ret += sqrt(RND.mwc32());
            }
            // cout << " j ";
            // do_flops(5000);
            tmp_count = TimeUtil::convert_us(TimeUtil::getTime() - start);
         }


         count += tmp_count;
      }
      // ++jj;
      // if (j==0)  cout << "-" << kk << "-" << j << "4 " << endl;
   } while (count < wcet_us);

   dummy((void *)&ret);
   count = 0;
   return(true);
}
