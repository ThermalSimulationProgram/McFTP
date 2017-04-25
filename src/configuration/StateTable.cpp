#include "configuration/StateTable.h"

#include <iostream>
#include <cstdlib>

using namespace std;

StateTable::StateTable(int _coreId):frequencies(),lengths(){
	if (_coreId < 0){
		cout << "StateTable::StateTable: input core index must be nonnegative" << endl;
		exit(1);
	}
	frequencies.reserve(100);
	lengths.reserve(100);
}

StateTable::~StateTable(){}

void StateTable::pushState(double f, unsigned long l){
	if (f < 0){
		cerr << "StateTable::pushState: input frequency must be nonnegative" << endl;
		exit(1);
	}
	// minimal interval length: 100 microseconds
	if (l > 100){
		frequencies.push_back(f);
		lengths.push_back(l);
	}
}


void StateTable::getState(int id, double& f, unsigned long& l){
	f = getFrequency(id);
	l = getLength(id);
}

double StateTable::getFrequency(int id){
	if (id < 0 || id >= (int)frequencies.size()){
		cerr << "StateTable::getFrequency: state id exceeds range" << endl;
		exit(1);
	}

	return frequencies[id];
}

unsigned long StateTable::getLength(int id){
	if (id < 0 || id >= (int)lengths.size()){
		cerr << "StateTable::getLength: state id exceeds range" << endl;
		exit(1);
	}

	return lengths[id];
}

int StateTable::getTargetCoreId() const {
	return coreId;
}

unsigned StateTable::getSize(){
	return lengths.size();
}