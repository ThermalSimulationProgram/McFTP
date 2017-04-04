#include "StateTable.h"

#include <iostream>
#include <cstdlib>

using namespace std;

StableTable::StableTable(int _coreId):frequencies(),lengths(),
	stateRowId(0), coreId(_coreId){
	frequencies.reserve(100);
	lengths.reserve(100);
	stateEndTimes.reserve(100);
}

StableTable::~StableTable(){}

void StableTable::pushState(double f, long l){
	if (f < 0){
		cerr << "StableTable::pushState: input frequency must be nonnegative" << endl;
		exit(1);
	}

	if (l < 0){
		cerr << "StableTable::pushState: input time length must be nonnegative" << endl;
		exit(1);
	}

	frequencies.push_back(f);
	lengths.push_back(l);

}


void StableTable::getState(double& f, long& l){
	f = frequencies[stateRowId];
	l = lengths[stateRowId];
	++stateRowId;
	if (stateRowId >= lengths.size()){
		stateRowId = 0;
	}
}

void StableTable::setStateId(int id){
	if (id >= lengths.size() || id < 0){
		id = 0;
	}

	stateRowId = id;
}