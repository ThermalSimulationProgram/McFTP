#include "utils/TemperatureSaveOption.h"

#include <iostream>

#include "utils/utils.h"

using namespace std;



TemperatureSaveOption::TemperatureSaveOption()
{
   isSaveGlobalPeakTemperature  = true;
   isSaveSensorPeakTemperature  = true;
   isSaveSensorMeanTemperature  = true;
   isSaveSensorReadOverhead     = true;
   isSaveSensorTemperatureTrace = true;
}

TemperatureSaveOption::~TemperatureSaveOption()
{
};

void TemperatureSaveOption::dump()
{
   cout << "isSaveGlobalPeakTemperature    = " << isSaveGlobalPeakTemperature << endl;
   cout << "isSaveSensorPeakTemperature    = " << isSaveSensorPeakTemperature << endl;
   cout << "isSaveSensorMeanTemperature    = " << isSaveSensorMeanTemperature << endl;
   cout << "isSaveSensorReadOverhead       = " << isSaveSensorReadOverhead << endl;
   cout << "isSaveSensorTemperatureTrace   = " << isSaveSensorTemperatureTrace << endl;
}

void TemperatureSaveOption::print(std::string name)
{
   cout << name << ": " << endl;
   dump();
   dumpLinesSeperator();
}
