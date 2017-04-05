#include "HyperStateTable.h"

#include "TimeUtil.h"
#include "Operators.h"

HyperStateTable::HyperStateTable():
 statetable(0){
 	rowId = 0;
 	tableSize = 0;
}

void HyperStateTable::setStateTable(const StateTable& s, const struct timespec& now){
	nextActionTime = now;
	statetable = s;
	tableSize = statetable.getSize();
 	rowId = 0;
}

double HyperStateTable::getState(){
	double ret;
	if (tableSize > 0){
		ret = statetable.getFrequency(rowId);
		unsigned long lastLength = statetable.getLength(rowId);
		nextActionTime = nextActionTime + TimeUtil::Micros(lastLength);

		if (rowId >= tableSize - 1){
			rowId = 0;
		}else{
			++rowId;
		}
	}else{
		ret = -1; // default: no control
	}
	
	return ret;
}


struct timespec HyperStateTable::peekNextTime(){
	return nextActionTime;
}