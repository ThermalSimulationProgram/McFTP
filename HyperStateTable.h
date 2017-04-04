#ifndef HYPERSTATETABLE
#define HYPERSTATETABLE

#include <StateTable.h>
#include <time.h>


class HyperStateTable: public {

private:
	StateTable statetable;
	std::vector<struct timespec> stateEndTimes;

public:
	HyperStateTable();
	~HyperStateTable();

	

};


#endif