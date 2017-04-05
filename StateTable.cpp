#include "StateTable.h"

#include <iostream>
#include <cstdlib>

using namespace std;

StateTable::StateTable(int _coreId):frequencies(),lengths(),
	coreId(_coreId){
	frequencies.reserve(100);
	lengths.reserve(100);
}

StateTable::~StateTable(){}

void StateTable::pushState(double f, unsigned long l){
	if (f < 0){
		cerr << "StateTable::pushState: input frequency must be nonnegative" << endl;
		exit(1);
	}

	frequencies.push_back(f);
	lengths.push_back(l);

}


void StateTable::getState(int id, double& f, unsigned long& l){

	f = getFrequency(id);
	l = getLength(id);
}

double StateTable::getFrequency(int id){
	if (id < 0 || id >= frequencies.size()){
		cerr << "StateTable::getFrequency: state id exceeds range" << endl;
		exit(1);
	}

	return frequencies[id];
}

unsigned long StateTable::getLength(int id){
	if (id < 0 || id >= lengths.size()){
		cerr << "StateTable::getLength: state id exceeds range" << endl;
		exit(1);
	}

	return lengths[id];
}

// void StateTable::setStateId(int id){
// 	if (id >= lengths.size() || id < 0){
// 		id = 0;
// 	}

// 	stateRowId = id;
// }


unsigned StateTable::getSize(){
	return lengths.size();
}