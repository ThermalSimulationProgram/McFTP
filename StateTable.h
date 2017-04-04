#ifndef STATETABLE_H
#define STATETABLE_H

#include <vector>


class StableTable{

private:
	// vector storing the sequency of frequencies for a core. The frequency must 
	// be zero or one of the feasible frequencies.
	std::vector<double> frequencies;

	// vector storing the time lengths of the states, unit microsecond
	std::vector<long> lengths;

	int coreId;

	// auxiliary varible representing current state id.
	int stateRowId;

public:
	StableTable();

	~StableTable();

	void pushState(double f, long l);

	void getState(double & f, long & l);

	void setStateId(int);


};

























#endif