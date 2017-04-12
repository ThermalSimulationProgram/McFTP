#include "ThermalApproachAPI/UserDefinedLoad.h"

#include <time.h>

#include "utils/Operators.h"
#include "utils/TimeUtil.h"


UserDefinedLoad::UserDefinedLoad(int _loadId):TaskLoad(){
	count = 0;
	loadId = _loadId;
}

UserDefinedLoad::~UserDefinedLoad(){}


void UserDefinedLoad::runLoads(unsigned long _wcet_us){
	wcet_us = _wcet_us;
	switch (loadId){
		case 1:{
			coolTask();
			break;
		}

		case 2:{}
		case 3:{}
		
	}
	


	
	

}


void UserDefinedLoad::coolTask(){
	unsigned long fire_start = TimeUtil::convert_us(TimeUtil::getTime());
	struct timespec sleep = TimeUtil::Micros(200);
	struct timespec rem;

	do {
     setSuspendPoint(); // set a suspend point here. When recieves a suspend signal, pause execution here
     setStopPoint();
    
     nanosleep(&sleep, &rem);


     unsigned long end = TimeUtil::convert_us(TimeUtil::getTime());
     count += end - fire_start;
     fire_start = end;
 } while ( count < wcet_us); 

}

