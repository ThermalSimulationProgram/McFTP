#include "taskloads/BusyWait.h"

#include "utils/Operators.h"
#include "utils/TimeUtil.h"


BusyWait::BusyWait():TaskLoad(){
	count = 0;
}

BusyWait::~BusyWait(){}


void BusyWait::runLoads(unsigned long _wcet_us){

     wcet_us = _wcet_us;

	unsigned long fire_start = TimeUtil::convert_us(TimeUtil::getTime());

	do {
     setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
     setStopPoint();
     
     unsigned long start = 0, tmp_count = 0;
     start = TimeUtil::convert_us(TimeUtil::getTime());
     while(tmp_count < 200){
     	tmp_count = TimeUtil::convert_us(TimeUtil::getTime()) - start;
     }

     unsigned long end = TimeUtil::convert_us(TimeUtil::getTime());
     count += end - fire_start;
     fire_start = end;
 } while ( count < wcet_us); 

}


