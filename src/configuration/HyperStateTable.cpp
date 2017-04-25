#include "configuration/HyperStateTable.h"

#include <iostream>
#include <climits>

#include "utils/TimeUtil.h"
#include "utils/Operators.h"


HyperStateTable::HyperStateTable():
 statetable(0){
 	rowId = 0;
 	tableSize = 0;
 	nextActionTime.tv_sec = LONG_MAX;
 	nextActionTime.tv_nsec = 999999999;

}

HyperStateTable::~HyperStateTable(){
	
}

void HyperStateTable::setStateTable(const StateTable& s, const struct timespec& now){
	nextActionTime = now;
	statetable = s;
	tableSize = statetable.getSize();
 	rowId = 0;
}

double HyperStateTable::getState(const  struct timespec& now, struct timespec& length){
	double ret;
	if (tableSize > 0){
		ret = statetable.getFrequency(rowId);
		unsigned long lastLength = statetable.getLength(rowId);
		// std::cout << "lastLength: " << lastLength << std::endl;
		
		length = TimeUtil::Micros(lastLength);
		nextActionTime = nextActionTime + length;

		if (rowId >= tableSize - 1){
			rowId = 0;
		}else{
			++rowId;
		}
	}else{
		length = TimeUtil::Micros(0);
		ret = -1; // default: no control
	}
	
	return ret;
}


struct timespec HyperStateTable::peekNextTime(){
	return nextActionTime;
}