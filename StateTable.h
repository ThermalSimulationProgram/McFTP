#ifndef STATETABLE_H
#define STATETABLE_H

#include <vector>


class StateTable{

protected:
	// vector storing the sequency of frequencies for a core. The frequency must 
	// be zero or one of the feasible frequencies.
	std::vector<double> frequencies;

	// vector storing the time lengths of the states, unit microsecond
	std::vector<unsigned long> lengths;

	int coreId;

	// // auxiliary varible representing current state id.
	// int stateRowId;

public:
	StateTable(int);

	~StateTable();

	void pushState(double f, unsigned long l);

	void getState(int id, double & f, unsigned long & l);

	double getFrequency(int id);

	unsigned long getLength(int id);

	// void setStateId(int);

	unsigned getSize();


};

























#endif