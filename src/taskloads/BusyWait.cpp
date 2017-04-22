#include "taskloads/BusyWait.h"

#include <cmath>

#include "utils/Operators.h"
#include "utils/TimeUtil.h"

#include "taskloads/MWC.h"


BusyWait::BusyWait():TaskLoad(){
	count = 0;
}

BusyWait::~BusyWait(){}


void BusyWait::runLoads(unsigned long _wcet_us){
     initCheckCounter();
     unsigned long fire_start = TimeUtil::convert_us(TimeUtil::getTime());
     wcet_us = _wcet_us;
     MWC RND = MWC();



     do {
          setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
          setCheckBlockBegin();
     
          unsigned long start = 0, tmp_count = 0;
          start = TimeUtil::convert_us(TimeUtil::getTime());
          while(tmp_count < 200){
               for (int i = 0; i < 3000; ++i)
               {
                    sqrt(RND.mwc32());
               }
               tmp_count = TimeUtil::convert_us(TimeUtil::getTime()) - start;
          }

          unsigned long end = TimeUtil::convert_us(TimeUtil::getTime());
          count += end - fire_start;
          fire_start = end;
          setCheckBlockEnd();
     } while ( count < wcet_us); 

     count = 0;

}


