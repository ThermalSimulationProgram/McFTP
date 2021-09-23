#include "utils/SoftTemperatureSaveOption.h"

#include <iostream>

#include "utils/utils.h"

using namespace std;

SoftTemperatureSaveOption::SoftTemperatureSaveOption() : TemperatureSaveOption()
{
   isSavePerformanceCounterValues = true;
}

SoftTemperatureSaveOption::~SoftTemperatureSaveOption()
{
}

void SoftTemperatureSaveOption::print(std::string name)
{
   cout << name << ": " << endl;
   TemperatureSaveOption::dump();
   cout << "isSavePerformanceCounterValues = " << isSavePerformanceCounterValues << endl;
   dumpLinesSeperator();
}
