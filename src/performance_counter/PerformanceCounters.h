#ifndef MCFTP_PERFORMANCECOUNTERS_H
#define MCFTP_PERFORMANCECOUNTERS_H 

#define MAX_EVENT_COUNTER_NUMBER 100

#include <vector>
#include <string>

class PerformanceCounters{
protected:
	bool initialized;

	int counters[MAX_EVENT_COUNTER_NUMBER];

	std::vector<std::string> counterNames;

	long long values[MAX_EVENT_COUNTER_NUMBER];

	int counterNumber;


public:

	PerformanceCounters();

	bool initialize();

	bool addCounter(const std::string counterName);

	bool startAllCounters();

	bool readAllValues();

	long long getCounterValue(const std::string counterName);

	long long getCounterValue(int counterId);

	int getCounterNumber();

	static void printCounterInfo();

	static void PAPI_test(int argc, char **argv);

};

#endif